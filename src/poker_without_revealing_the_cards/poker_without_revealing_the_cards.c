#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sodium.h>

#define DECK_SIZE 52
#define HOLE_CARDS 2
#define COMMUNITY_CARDS 5
#define TOTAL_DEALT_CARDS ((2 * HOLE_CARDS) + COMMUNITY_CARDS)
#define PRIME_MODULUS 104729ULL

#define HAND_NONCE_BYTES 16
#define HAND_COMMITMENT_BYTES 32
#define DRAW_COMMITMENT_BYTES 32

typedef enum {
    SUIT_MODE_ASCII = 0,
    SUIT_MODE_UNICODE = 1
} SuitMode;

typedef struct {
    const char *name;
    uint64_t e;
    uint64_t d;
    uint64_t hole_cards[HOLE_CARDS];
    uint8_t hand_nonce[HAND_NONCE_BYTES];
    uint8_t hand_commitment[HAND_COMMITMENT_BYTES];
} Player;

typedef struct {
    const char *label;
    uint8_t commitment[DRAW_COMMITMENT_BYTES];
} DrawSample;

static const char *C_RESET = "";
static const char *C_BOLD = "";
static const char *C_DIM = "";
static const char *C_INFO = "";
static const char *C_STAGE = "";
static const char *C_OK = "";
static const char *C_WARN = "";
static const char *C_ERR = "";
static const char *C_CARD_RED = "";
static const char *C_CARD_BLACK = "";
static SuitMode G_SUIT_MODE = SUIT_MODE_ASCII;
static const char *DRAW_LABELS[TOTAL_DEALT_CARDS] = {
    "Alice hole card 1",
    "Alice hole card 2",
    "Bob hole card 1",
    "Bob hole card 2",
    "Community card 1",
    "Community card 2",
    "Community card 3",
    "Community card 4",
    "Community card 5"
};

static void fatal(const char *message);

static void init_log_colors(void) {
    const char *term = getenv("TERM");
    const char *no_color = getenv("NO_COLOR");
    int use_color = (isatty(STDOUT_FILENO) != 0) && (no_color == NULL) &&
                    (term != NULL) && (strcmp(term, "dumb") != 0);

    if (!use_color) {
        return;
    }

    C_RESET = "\x1b[0m";
    C_BOLD = "\x1b[1m";
    C_DIM = "\x1b[2m";
    C_INFO = "\x1b[36m";
    C_STAGE = "\x1b[95m";
    C_OK = "\x1b[92m";
    C_WARN = "\x1b[93m";
    C_ERR = "\x1b[91m";
    C_CARD_RED = "\x1b[91m";
    C_CARD_BLACK = "\x1b[96m";
}

static void print_rule(char ch) {
    for (int i = 0; i < 72; ++i) {
        putchar(ch);
    }
    putchar('\n');
}

static void log_line(const char *color, const char *tag, const char *fmt, ...) {
    va_list args;

    printf("%s[%s]%s ", color, tag, C_RESET);
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');
}

static int string_contains_utf8_marker(const char *value) {
    if (value == NULL) {
        return 0;
    }
    if (strstr(value, "UTF-8") != NULL || strstr(value, "utf-8") != NULL) {
        return 1;
    }
    if (strstr(value, "UTF8") != NULL || strstr(value, "utf8") != NULL) {
        return 1;
    }
    return 0;
}

static int locale_supports_utf8(void) {
    const char *lc_all = getenv("LC_ALL");
    const char *lc_ctype = getenv("LC_CTYPE");
    const char *lang = getenv("LANG");

    return string_contains_utf8_marker(lc_all) ||
           string_contains_utf8_marker(lc_ctype) ||
           string_contains_utf8_marker(lang);
}

static SuitMode default_suit_mode(void) {
    if ((isatty(STDOUT_FILENO) != 0) && locale_supports_utf8()) {
        return SUIT_MODE_UNICODE;
    }
    return SUIT_MODE_ASCII;
}

static int try_parse_suit_mode(const char *value, SuitMode *out_mode) {
    if (value == NULL || out_mode == NULL) {
        return 0;
    }
    if (strcmp(value, "ascii") == 0 || strcmp(value, "letters") == 0) {
        *out_mode = SUIT_MODE_ASCII;
        return 1;
    }
    if (strcmp(value, "unicode") == 0 || strcmp(value, "utf8") == 0 ||
        strcmp(value, "utf-8") == 0) {
        *out_mode = SUIT_MODE_UNICODE;
        return 1;
    }
    return 0;
}

static void print_usage(const char *program_name) {
    printf("Usage: %s [--suits ascii|unicode]\n", program_name);
    printf("Env override: POKER_SUIT_MODE=ascii|unicode\n");
}

static void configure_suit_mode_from_args(int argc, char **argv) {
    SuitMode mode = default_suit_mode();
    const char *env_mode = getenv("POKER_SUIT_MODE");

    if (env_mode != NULL) {
        if (!try_parse_suit_mode(env_mode, &mode)) {
            fatal("invalid POKER_SUIT_MODE; expected ascii or unicode");
        }
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        }
        if (strcmp(argv[i], "--suits") == 0) {
            if (i + 1 >= argc) {
                fatal("missing value for --suits (ascii|unicode)");
            }
            if (!try_parse_suit_mode(argv[i + 1], &mode)) {
                fatal("invalid --suits value; expected ascii or unicode");
            }
            ++i;
            continue;
        }
        fatal("unknown argument; use --help for usage");
    }

    G_SUIT_MODE = mode;
}

static void fatal(const char *message) {
    fprintf(stderr, "%s[error]%s %s\n", C_ERR, C_RESET, message);
    exit(1);
}

static uint64_t random_bounded_u64(uint64_t bound_exclusive) {
    if (bound_exclusive == 0ULL || bound_exclusive > UINT32_MAX) {
        fatal("random bound is out of supported range");
    }
    return (uint64_t)randombytes_uniform((uint32_t)bound_exclusive);
}

static uint64_t gcd_u64(uint64_t a, uint64_t b) {
    while (b != 0ULL) {
        uint64_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}

static uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1ULL % mod;
    base %= mod;

    while (exp > 0ULL) {
        if ((exp & 1ULL) != 0ULL) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp >>= 1U;
    }

    return result;
}

static uint64_t mod_inverse(uint64_t value, uint64_t mod) {
    int64_t t = 0;
    int64_t new_t = 1;
    int64_t r = (int64_t)mod;
    int64_t new_r = (int64_t)(value % mod);

    while (new_r != 0) {
        int64_t quotient = r / new_r;
        int64_t temp_t = t - (quotient * new_t);
        int64_t temp_r = r - (quotient * new_r);
        t = new_t;
        new_t = temp_t;
        r = new_r;
        new_r = temp_r;
    }

    if (r != 1) {
        return 0ULL;
    }
    if (t < 0) {
        t += (int64_t)mod;
    }
    return (uint64_t)t;
}

static uint64_t choose_exponent(uint64_t phi) {
    uint64_t candidate = 0ULL;

    do {
        candidate = 3ULL + random_bounded_u64(phi - 3ULL);
        candidate |= 1ULL;
    } while (candidate >= phi || gcd_u64(candidate, phi) != 1ULL);

    return candidate;
}

static uint64_t hash_u64_array(const uint64_t *values, size_t count) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < count; ++i) {
        uint64_t value = values[i];
        for (int byte = 0; byte < 8; ++byte) {
            hash ^= (value >> (uint64_t)(byte * 8)) & 0xFFULL;
            hash *= prime;
        }
    }

    return hash;
}

static void init_clear_deck(uint64_t *deck) {
    for (size_t i = 0; i < DECK_SIZE; ++i) {
        deck[i] = (uint64_t)i + 2ULL;
    }
}

static void shuffle_u64(uint64_t *values, size_t count) {
    if (count < 2U) {
        return;
    }

    for (size_t i = count - 1U; i > 0U; --i) {
        size_t j = (size_t)random_bounded_u64((uint64_t)i + 1ULL);
        uint64_t tmp = values[i];
        values[i] = values[j];
        values[j] = tmp;
    }
}

static void encrypt_deck(uint64_t *deck, uint64_t exponent) {
    for (size_t i = 0; i < DECK_SIZE; ++i) {
        deck[i] = mod_pow(deck[i], exponent, PRIME_MODULUS);
    }
}

static uint64_t draw_encrypted(const uint64_t *deck, size_t *cursor) {
    if (*cursor >= DECK_SIZE) {
        fatal("deck exhausted");
    }
    return deck[(*cursor)++];
}

static uint64_t decrypt_for_owner(uint64_t encrypted_card, const Player *other,
                                  const Player *owner) {
    uint64_t without_other = mod_pow(encrypted_card, other->d, PRIME_MODULUS);
    return mod_pow(without_other, owner->d, PRIME_MODULUS);
}

static uint64_t decrypt_public(uint64_t encrypted_card, const Player *alice,
                               const Player *bob) {
    uint64_t without_alice = mod_pow(encrypted_card, alice->d, PRIME_MODULUS);
    return mod_pow(without_alice, bob->d, PRIME_MODULUS);
}

static int card_to_index(uint64_t card) {
    if (card < 2ULL || card > 53ULL) {
        return -1;
    }
    return (int)(card - 2ULL);
}

static void mark_seen(const char *label, uint64_t card, int *seen_cards) {
    int index = card_to_index(card);
    if (index < 0 || index >= DECK_SIZE) {
        fprintf(stderr, "Invalid decrypted card in %s: %" PRIu64 "\n", label, card);
        exit(1);
    }
    if (seen_cards[index] != 0) {
        fprintf(stderr, "Duplicated card detected in %s.\n", label);
        exit(1);
    }
    seen_cards[index] = 1;
}

static void card_to_string(uint64_t card, char *buffer, size_t buffer_size) {
    static const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8",
                                  "9", "T", "J", "Q", "K", "A"};
    static const char *suits_ascii[] = {"C", "D", "H", "S"};
    static const char *suits_unicode[] = {
        "\xE2\x99\xA3", /* ♣ */
        "\xE2\x99\xA6", /* ♦ */
        "\xE2\x99\xA5", /* ♥ */
        "\xE2\x99\xA0"  /* ♠ */
    };
    const char **suits = (G_SUIT_MODE == SUIT_MODE_UNICODE) ? suits_unicode : suits_ascii;

    if (card < 2ULL || card > 53ULL) {
        (void)snprintf(buffer, buffer_size, "??");
        return;
    }

    int card_index = (int)(card - 2ULL);
    int rank = card_index % 13;
    int suit = card_index / 13;

    (void)snprintf(buffer, buffer_size, "%s%s", ranks[rank], suits[suit]);
}

static const char *color_for_card(uint64_t card) {
    if (card < 2ULL || card > 53ULL) {
        return C_DIM;
    }

    int suit = ((int)(card - 2ULL)) / 13;
    if (suit == 1 || suit == 2) {
        return C_CARD_RED;
    }
    return C_CARD_BLACK;
}

static void print_card(uint64_t card) {
    char card_label[8];
    const char *color;

    card_to_string(card, card_label, sizeof(card_label));
    color = color_for_card(card);
    printf("%s%s%s", color, card_label, C_RESET);
}

static void print_card_list(const char *title, const uint64_t *cards, size_t count) {
    printf("%s%s%s: ", C_BOLD, title, C_RESET);
    for (size_t i = 0; i < count; ++i) {
        print_card(cards[i]);
        printf("%s", (i + 1U < count) ? " " : "");
    }
    printf("\n");
}

static void print_encrypted_preview(const uint64_t *deck, size_t count) {
    printf("%s[trace]%s Encrypted top-of-deck preview: ", C_INFO, C_RESET);
    for (size_t i = 0; i < count; ++i) {
        printf("%" PRIu64 "%s", deck[i], (i + 1U < count) ? " " : "");
    }
    printf("\n");
}

static void print_hex(const uint8_t *bytes, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%02x", bytes[i]);
    }
}

static void encode_u64_le(uint64_t value, uint8_t out[8]) {
    for (size_t i = 0; i < 8U; ++i) {
        out[i] = (uint8_t)((value >> (8U * i)) & 0xFFULL);
    }
}

static void compute_draw_commitment(const char *label, uint64_t encrypted_card,
                                    uint8_t out[DRAW_COMMITMENT_BYTES]) {
    crypto_generichash_state state;
    uint8_t encoded_card[8];

    if (crypto_generichash_init(&state, NULL, 0U, DRAW_COMMITMENT_BYTES) != 0) {
        fatal("failed to initialize draw commitment hash state");
    }

    crypto_generichash_update(&state, (const unsigned char *)label, strlen(label));
    encode_u64_le(encrypted_card, encoded_card);
    crypto_generichash_update(&state, encoded_card, sizeof(encoded_card));

    if (crypto_generichash_final(&state, out, DRAW_COMMITMENT_BYTES) != 0) {
        fatal("failed to finalize draw commitment hash");
    }
}

static void publish_draw_samples(const uint64_t *deck, DrawSample *samples) {
    log_line(C_INFO, "info",
             "Publishing hashed commitments for the encrypted cards that will be dealt.");

    for (size_t i = 0; i < TOTAL_DEALT_CARDS; ++i) {
        samples[i].label = DRAW_LABELS[i];
        compute_draw_commitment(samples[i].label, deck[i], samples[i].commitment);
        printf("%s[sample]%s %s: %s", C_WARN, C_RESET, samples[i].label, C_DIM);
        print_hex(samples[i].commitment, DRAW_COMMITMENT_BYTES);
        printf("%s\n", C_RESET);
    }
}

static void verify_draw_sample(const DrawSample *sample, uint64_t encrypted_card) {
    uint8_t expected[DRAW_COMMITMENT_BYTES];

    compute_draw_commitment(sample->label, encrypted_card, expected);
    if (sodium_memcmp(expected, sample->commitment, DRAW_COMMITMENT_BYTES) != 0) {
        fatal("draw sample commitment verification failed");
    }

    log_line(C_OK, "sample", "Verified sampled ciphertext for %s.", sample->label);
}

static void compute_hand_commitment(const uint64_t *cards, size_t count,
                                    const uint8_t nonce[HAND_NONCE_BYTES],
                                    uint8_t out[HAND_COMMITMENT_BYTES]) {
    crypto_generichash_state state;

    if (crypto_generichash_init(&state, NULL, 0U, HAND_COMMITMENT_BYTES) != 0) {
        fatal("failed to initialize commitment hash state");
    }

    crypto_generichash_update(&state, nonce, HAND_NONCE_BYTES);

    uint8_t count_byte = (uint8_t)count;
    crypto_generichash_update(&state, &count_byte, 1U);

    for (size_t i = 0; i < count; ++i) {
        uint8_t encoded_card[8];
        uint64_t card = cards[i];
        for (size_t b = 0; b < 8U; ++b) {
            encoded_card[b] = (uint8_t)((card >> (8U * b)) & 0xFFULL);
        }
        crypto_generichash_update(&state, encoded_card, sizeof(encoded_card));
    }

    if (crypto_generichash_final(&state, out, HAND_COMMITMENT_BYTES) != 0) {
        fatal("failed to finalize commitment hash");
    }
}

static void commit_player_hand(Player *player) {
    randombytes_buf(player->hand_nonce, HAND_NONCE_BYTES);
    compute_hand_commitment(player->hole_cards, HOLE_CARDS, player->hand_nonce,
                            player->hand_commitment);
}

static int verify_player_commitment(const Player *player) {
    uint8_t expected[HAND_COMMITMENT_BYTES];
    compute_hand_commitment(player->hole_cards, HOLE_CARDS, player->hand_nonce, expected);
    return sodium_memcmp(expected, player->hand_commitment, HAND_COMMITMENT_BYTES) == 0;
}

static void print_stage_header(int stage, const char *title) {
    putchar('\n');
    print_rule('=');
    printf("%s%sStage %d%s\n", C_BOLD, C_STAGE, stage, C_RESET);
    printf("%s%s%s\n", C_STAGE, title, C_RESET);
    print_rule('-');
}

static void stage1_randomize_deck(uint64_t *deck) {
    print_stage_header(1, "Randomization of the deck");
    init_clear_deck(deck);
    shuffle_u64(deck, DECK_SIZE);
    log_line(C_INFO, "info", "Initialized and shuffled a 52-card deck.");
    log_line(C_WARN, "commit", "Public deck-order commitment: 0x%016" PRIx64,
             hash_u64_array(deck, DECK_SIZE));
    log_line(C_OK, "ok", "Deck now has a hidden permutation.");
}

static void stage2_secure_multiparty_shuffle(uint64_t *deck, Player *alice, Player *bob,
                                             DrawSample *samples) {
    uint64_t phi = PRIME_MODULUS - 1ULL;

    print_stage_header(2, "Secure multi-party shuffling");
    log_line(C_INFO, "info", "Prime modulus: %" PRIu64 ", phi(modulus): %" PRIu64,
             (uint64_t)PRIME_MODULUS, phi);

    alice->e = choose_exponent(phi);
    alice->d = mod_inverse(alice->e, phi);
    do {
        bob->e = choose_exponent(phi);
    } while (bob->e == alice->e);
    bob->d = mod_inverse(bob->e, phi);

    if (alice->d == 0ULL || bob->d == 0ULL) {
        fatal("failed to initialize commutative cipher keys");
    }

    shuffle_u64(deck, DECK_SIZE);
    encrypt_deck(deck, alice->e);
    log_line(C_WARN, "commit", "%s encrypted-deck commitment: 0x%016" PRIx64, alice->name,
             hash_u64_array(deck, DECK_SIZE));

    shuffle_u64(deck, DECK_SIZE);
    encrypt_deck(deck, bob->e);
    log_line(C_WARN, "commit", "%s encrypted-deck commitment:   0x%016" PRIx64, bob->name,
             hash_u64_array(deck, DECK_SIZE));

    print_encrypted_preview(deck, 8U);
    publish_draw_samples(deck, samples);
}

static void stage3_secure_multiparty_deal(const uint64_t *deck, const DrawSample *samples,
                                          Player *alice, Player *bob,
                                          uint64_t *community_cards) {
    int seen_cards[DECK_SIZE] = {0};
    size_t cursor = 0U;
    size_t sample_index = 0U;

    print_stage_header(3, "Secure multi-party dealing");

    for (size_t i = 0; i < HOLE_CARDS; ++i) {
        uint64_t encrypted_card = draw_encrypted(deck, &cursor);
        verify_draw_sample(&samples[sample_index], encrypted_card);
        ++sample_index;
        alice->hole_cards[i] = decrypt_for_owner(encrypted_card, bob, alice);
        mark_seen("Alice hole cards", alice->hole_cards[i], seen_cards);
    }

    for (size_t i = 0; i < HOLE_CARDS; ++i) {
        uint64_t encrypted_card = draw_encrypted(deck, &cursor);
        verify_draw_sample(&samples[sample_index], encrypted_card);
        ++sample_index;
        bob->hole_cards[i] = decrypt_for_owner(encrypted_card, alice, bob);
        mark_seen("Bob hole cards", bob->hole_cards[i], seen_cards);
    }

    for (size_t i = 0; i < COMMUNITY_CARDS; ++i) {
        uint64_t encrypted_card = draw_encrypted(deck, &cursor);
        verify_draw_sample(&samples[sample_index], encrypted_card);
        ++sample_index;
        community_cards[i] = decrypt_public(encrypted_card, alice, bob);
        mark_seen("Community cards", community_cards[i], seen_cards);
    }

    if (sample_index != TOTAL_DEALT_CARDS) {
        fatal("unexpected sampled-card count during dealing");
    }

    print_card_list("Alice hole cards (private in real deployment)", alice->hole_cards,
                    HOLE_CARDS);
    print_card_list("Bob hole cards   (private in real deployment)", bob->hole_cards,
                    HOLE_CARDS);
    print_card_list("Community cards (public)", community_cards, COMMUNITY_CARDS);
    log_line(C_INFO, "info", "Dealt cards: %u",
             (unsigned int)(2U * HOLE_CARDS + COMMUNITY_CARDS));
    log_line(C_OK, "ok", "Integrity check: no duplicated cards were detected.");
}

static void stage4_publish_hand_commitments(const Player *alice, const Player *bob) {
    int alice_valid;
    int bob_valid;

    print_stage_header(4, "Hand commitments and validation");

    printf("%s[commit]%s %s publishes commitment: %s", C_WARN, C_RESET, alice->name, C_DIM);
    print_hex(alice->hand_commitment, HAND_COMMITMENT_BYTES);
    printf("%s\n", C_RESET);

    printf("%s[commit]%s %s publishes commitment:   %s", C_WARN, C_RESET, bob->name, C_DIM);
    print_hex(bob->hand_commitment, HAND_COMMITMENT_BYTES);
    printf("%s\n", C_RESET);

    log_line(C_INFO, "info",
             "At showdown each player reveals cards + nonce for validation.");

    alice_valid = verify_player_commitment(alice);
    printf("%s[reveal]%s %s nonce: %s", C_INFO, C_RESET, alice->name, C_DIM);
    print_hex(alice->hand_nonce, HAND_NONCE_BYTES);
    printf("%s | verification: %s%s%s\n", C_RESET,
           alice_valid ? C_OK : C_ERR, alice_valid ? "valid" : "invalid", C_RESET);

    bob_valid = verify_player_commitment(bob);
    printf("%s[reveal]%s %s nonce:   %s", C_INFO, C_RESET, bob->name, C_DIM);
    print_hex(bob->hand_nonce, HAND_NONCE_BYTES);
    printf("%s | verification: %s%s%s\n", C_RESET,
           bob_valid ? C_OK : C_ERR, bob_valid ? "valid" : "invalid", C_RESET);
}

int main(int argc, char **argv) {
    uint64_t deck[DECK_SIZE];
    uint64_t community_cards[COMMUNITY_CARDS];
    DrawSample draw_samples[TOTAL_DEALT_CARDS];
    Player alice = {.name = "Alice", .e = 0ULL, .d = 0ULL, .hole_cards = {0ULL}};
    Player bob = {.name = "Bob", .e = 0ULL, .d = 0ULL, .hole_cards = {0ULL}};

    init_log_colors();
    configure_suit_mode_from_args(argc, argv);

    if (sodium_init() < 0) {
        fatal("libsodium initialization failed");
    }

    print_rule('=');
    printf("%s%sMENTAL POKER DEMO%s\n", C_BOLD, C_STAGE, C_RESET);
    log_line(C_INFO, "info", "Texas Hold'em staged protocol.");
    log_line(C_INFO, "info", "This run demonstrates Stage 1 to Stage 4.");
    log_line(C_INFO, "info", "Suit display mode: %s",
             (G_SUIT_MODE == SUIT_MODE_UNICODE) ? "unicode" : "ascii");

    stage1_randomize_deck(deck);
    stage2_secure_multiparty_shuffle(deck, &alice, &bob, draw_samples);
    stage3_secure_multiparty_deal(deck, draw_samples, &alice, &bob, community_cards);

    commit_player_hand(&alice);
    commit_player_hand(&bob);
    stage4_publish_hand_commitments(&alice, &bob);

    putchar('\n');
    print_rule('-');
    log_line(C_OK, "done", "Demo scope: secrecy, integrity, and commitments.");
    log_line(C_WARN, "next", "Deferred: betting protocol, hand ranking, networked peers.");
    print_rule('=');

    return 0;
}

include ../Makefile.config

TEST_BIN_DIR := $(BUILD_DIR)/tests
COMMITMENT_TEST_BIN := $(TEST_BIN_DIR)/test_commitment
RPS_TEST_BIN := $(TEST_BIN_DIR)/test_rps
RPS_ADVERSARIAL_TEST_BIN := $(TEST_BIN_DIR)/test_rps_adversarial
HASH_TEST_BIN := $(TEST_BIN_DIR)/test_hash
TTT_TEST_BIN := $(TEST_BIN_DIR)/test_ttt
BOOL_CIRCUIT_TEST_BIN := $(TEST_BIN_DIR)/test_bool_circuit
TTT_CORE_CIRCUIT_TEST_BIN := $(TEST_BIN_DIR)/test_ttt_core_circuit

COMMITMENT_SRC := $(SRC_DIR)/primitives/commitment.c
RPS_SRC := $(SRC_DIR)/protocols/rps.c
HASH_SRC := $(SRC_DIR)/primitives/hash.c
TTT_SRC := $(SRC_DIR)/protocols/ttt.c
BOOL_CIRCUIT_SRC := $(SRC_DIR)/circuits/bool_circuit.c
TTT_CORE_CIRCUIT_SRC := $(SRC_DIR)/circuits/ttt_core_circuit.c

TEST_CPPFLAGS := $(CPPFLAGS) -I$(INCLUDE_DIR) -I$(TESTS_DIR)

.PHONY: all test commitment rps adversarial hash ttt bool_circuit \
	ttt_core_circuit sanitize clean

all: $(COMMITMENT_TEST_BIN) $(RPS_TEST_BIN) $(RPS_ADVERSARIAL_TEST_BIN) \
	$(HASH_TEST_BIN) $(TTT_TEST_BIN) $(BOOL_CIRCUIT_TEST_BIN) \
	$(TTT_CORE_CIRCUIT_TEST_BIN)

test: all
	$(COMMITMENT_TEST_BIN)
	$(RPS_TEST_BIN)
	$(RPS_ADVERSARIAL_TEST_BIN)
	$(HASH_TEST_BIN)
	$(TTT_TEST_BIN)
	$(BOOL_CIRCUIT_TEST_BIN)
	$(TTT_CORE_CIRCUIT_TEST_BIN)

commitment: $(COMMITMENT_TEST_BIN)
	$(COMMITMENT_TEST_BIN)

rps: $(RPS_TEST_BIN)
	$(RPS_TEST_BIN)

adversarial: $(RPS_ADVERSARIAL_TEST_BIN)
	$(RPS_ADVERSARIAL_TEST_BIN)

hash: $(HASH_TEST_BIN)
	$(HASH_TEST_BIN)

ttt: $(TTT_TEST_BIN)
	$(TTT_TEST_BIN)

bool_circuit: $(BOOL_CIRCUIT_TEST_BIN)
	$(BOOL_CIRCUIT_TEST_BIN)

ttt_core_circuit: $(TTT_CORE_CIRCUIT_TEST_BIN)
	$(TTT_CORE_CIRCUIT_TEST_BIN)

$(COMMITMENT_TEST_BIN): test_commitment.c test_support.h $(COMMITMENT_SRC) \
		$(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/commitment.h | $(TEST_BIN_DIR)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) test_commitment.c $(COMMITMENT_SRC) \
		$(LDFLAGS) -o $@ $(LDLIBS)

$(RPS_TEST_BIN): test_rps.c test_support.h $(COMMITMENT_SRC) $(RPS_SRC) \
		$(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/commitment.h \
		$(INCLUDE_DIR)/ac/rps.h | $(TEST_BIN_DIR)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) test_rps.c $(COMMITMENT_SRC) $(RPS_SRC) \
		$(LDFLAGS) -o $@ $(LDLIBS)

$(RPS_ADVERSARIAL_TEST_BIN): test_rps_adversarial.c test_support.h \
		$(COMMITMENT_SRC) $(RPS_SRC) $(INCLUDE_DIR)/ac/status.h \
		$(INCLUDE_DIR)/ac/commitment.h $(INCLUDE_DIR)/ac/rps.h | $(TEST_BIN_DIR)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) test_rps_adversarial.c \
		$(COMMITMENT_SRC) $(RPS_SRC) $(LDFLAGS) -o $@ $(LDLIBS)

$(HASH_TEST_BIN): test_hash.c test_support.h $(HASH_SRC) \
		$(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/hash.h | $(TEST_BIN_DIR)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) test_hash.c $(HASH_SRC) \
		$(LDFLAGS) -o $@ $(LDLIBS)

$(TTT_TEST_BIN): test_ttt.c test_support.h $(TTT_SRC) \
		$(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/ttt.h | $(TEST_BIN_DIR)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) test_ttt.c $(TTT_SRC) \
		$(LDFLAGS) -o $@

$(BOOL_CIRCUIT_TEST_BIN): test_bool_circuit.c test_support.h \
		$(BOOL_CIRCUIT_SRC) $(INCLUDE_DIR)/ac/status.h \
		$(INCLUDE_DIR)/ac/bool_circuit.h | $(TEST_BIN_DIR)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) test_bool_circuit.c \
		$(BOOL_CIRCUIT_SRC) $(LDFLAGS) -o $@

$(TTT_CORE_CIRCUIT_TEST_BIN): test_ttt_core_circuit.c test_support.h \
		$(TTT_SRC) $(BOOL_CIRCUIT_SRC) $(TTT_CORE_CIRCUIT_SRC) \
		$(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/ttt.h \
		$(INCLUDE_DIR)/ac/bool_circuit.h \
		$(INCLUDE_DIR)/ac/ttt_core_circuit.h | $(TEST_BIN_DIR)
	$(CC) $(TEST_CPPFLAGS) $(CFLAGS) test_ttt_core_circuit.c \
		$(TTT_SRC) $(BOOL_CIRCUIT_SRC) $(TTT_CORE_CIRCUIT_SRC) \
		$(LDFLAGS) -o $@

$(TEST_BIN_DIR):
	mkdir -p $@

sanitize:
	ASAN_OPTIONS="detect_leaks=1:halt_on_error=1" \
	UBSAN_OPTIONS="halt_on_error=1" \
	$(MAKE) test \
		BUILD_DIR="$(BUILD_DIR)/sanitize" \
		CFLAGS="$(CFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer" \
		LDFLAGS="$(LDFLAGS) -fsanitize=address,undefined"

clean:
	rm -rf $(TEST_BIN_DIR)

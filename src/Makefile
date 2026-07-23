include ../Makefile.config

POKER_NAME := poker_without_revealing_the_cards
POKER_SRC := $(SRC_DIR)/$(POKER_NAME)/poker_without_revealing_the_cards.c
POKER_OBJ_DIR := $(TEMP_DIR)/$(POKER_NAME)
POKER_OBJ := $(POKER_OBJ_DIR)/poker_without_revealing_the_cards.o
POKER_BIN_DIR := $(BUILD_DIR)/$(POKER_NAME)
POKER_BIN := $(POKER_BIN_DIR)/poker_demo
POKER_ARGS ?=

RPS_NAME := two_oracles_play_rock_paper_scissors
COMMITMENT_SRC := $(SRC_DIR)/primitives/commitment.c
RPS_PROTOCOL_SRC := $(SRC_DIR)/protocols/rps.c
RPS_DEMO_SRC := $(SRC_DIR)/$(RPS_NAME)/rps_demo.c
RPS_OBJ_DIR := $(TEMP_DIR)/$(RPS_NAME)
COMMITMENT_OBJ := $(RPS_OBJ_DIR)/commitment.o
RPS_PROTOCOL_OBJ := $(RPS_OBJ_DIR)/rps.o
RPS_DEMO_OBJ := $(RPS_OBJ_DIR)/rps_demo.o
RPS_OBJS := $(COMMITMENT_OBJ) $(RPS_PROTOCOL_OBJ) $(RPS_DEMO_OBJ)
RPS_BIN_DIR := $(BUILD_DIR)/$(RPS_NAME)
RPS_BIN := $(RPS_BIN_DIR)/rps_demo
RPS_ARGS ?=

HASH_NAME := hash_functions
HASH_PRIMITIVE_SRC := $(SRC_DIR)/primitives/hash.c
HASH_DEMO_SRC := $(SRC_DIR)/$(HASH_NAME)/hash_demo.c
HASH_OBJ_DIR := $(TEMP_DIR)/$(HASH_NAME)
HASH_PRIMITIVE_OBJ := $(HASH_OBJ_DIR)/hash.o
HASH_DEMO_OBJ := $(HASH_OBJ_DIR)/hash_demo.o
HASH_OBJS := $(HASH_PRIMITIVE_OBJ) $(HASH_DEMO_OBJ)
HASH_BIN_DIR := $(BUILD_DIR)/$(HASH_NAME)
HASH_BIN := $(HASH_BIN_DIR)/hash_demo

TTT_NAME := tic_tac_toe_without_revealing_the_strategy
TTT_PROTOCOL_SRC := $(SRC_DIR)/protocols/ttt.c
BOOL_CIRCUIT_SRC := $(SRC_DIR)/circuits/bool_circuit.c
TTT_CORE_CIRCUIT_SRC := $(SRC_DIR)/circuits/ttt_core_circuit.c
TTT_DEMO_SRC := $(SRC_DIR)/$(TTT_NAME)/ttt_demo.c
TTT_OBJ_DIR := $(TEMP_DIR)/$(TTT_NAME)
TTT_PROTOCOL_OBJ := $(TTT_OBJ_DIR)/ttt.o
BOOL_CIRCUIT_OBJ := $(TTT_OBJ_DIR)/bool_circuit.o
TTT_CORE_CIRCUIT_OBJ := $(TTT_OBJ_DIR)/ttt_core_circuit.o
TTT_DEMO_OBJ := $(TTT_OBJ_DIR)/ttt_demo.o
TTT_OBJS := $(TTT_PROTOCOL_OBJ) $(BOOL_CIRCUIT_OBJ) \
	$(TTT_CORE_CIRCUIT_OBJ) $(TTT_DEMO_OBJ)
TTT_BIN_DIR := $(BUILD_DIR)/$(TTT_NAME)
TTT_BIN := $(TTT_BIN_DIR)/ttt_demo

.PHONY: all poker run_poker rps run_rps hash run_hash ttt run_ttt clean

all: poker rps hash ttt

poker: $(POKER_BIN)

run_poker: poker
	$(POKER_BIN) $(POKER_ARGS)

rps: $(RPS_BIN)

run_rps: rps
	$(RPS_BIN) $(RPS_ARGS)

hash: $(HASH_BIN)

run_hash: hash
	$(HASH_BIN)

ttt: $(TTT_BIN)

run_ttt: ttt
	$(TTT_BIN)

$(POKER_OBJ): $(POKER_SRC) | $(POKER_OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(POKER_BIN): $(POKER_OBJ) | $(POKER_BIN_DIR)
	$(CC) $(LDFLAGS) $< -o $@ $(LDLIBS)

$(COMMITMENT_OBJ): $(COMMITMENT_SRC) $(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/commitment.h | $(RPS_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(RPS_PROTOCOL_OBJ): $(RPS_PROTOCOL_SRC) $(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/commitment.h $(INCLUDE_DIR)/ac/rps.h | $(RPS_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(RPS_DEMO_OBJ): $(RPS_DEMO_SRC) $(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/commitment.h $(INCLUDE_DIR)/ac/rps.h | $(RPS_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(RPS_BIN): $(RPS_OBJS) | $(RPS_BIN_DIR)
	$(CC) $(LDFLAGS) $(RPS_OBJS) -o $@ $(LDLIBS)

$(HASH_PRIMITIVE_OBJ): $(HASH_PRIMITIVE_SRC) $(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/hash.h | $(HASH_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(HASH_DEMO_OBJ): $(HASH_DEMO_SRC) $(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/hash.h | $(HASH_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(HASH_BIN): $(HASH_OBJS) | $(HASH_BIN_DIR)
	$(CC) $(LDFLAGS) $(HASH_OBJS) -o $@ $(LDLIBS)

$(TTT_PROTOCOL_OBJ): $(TTT_PROTOCOL_SRC) $(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/ttt.h | $(TTT_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(BOOL_CIRCUIT_OBJ): $(BOOL_CIRCUIT_SRC) $(INCLUDE_DIR)/ac/status.h $(INCLUDE_DIR)/ac/bool_circuit.h | $(TTT_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(TTT_CORE_CIRCUIT_OBJ): $(TTT_CORE_CIRCUIT_SRC) $(INCLUDE_DIR)/ac/status.h \
		$(INCLUDE_DIR)/ac/ttt.h $(INCLUDE_DIR)/ac/bool_circuit.h \
		$(INCLUDE_DIR)/ac/ttt_core_circuit.h | $(TTT_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(TTT_DEMO_OBJ): $(TTT_DEMO_SRC) $(INCLUDE_DIR)/ac/status.h \
		$(INCLUDE_DIR)/ac/ttt.h $(INCLUDE_DIR)/ac/bool_circuit.h \
		$(INCLUDE_DIR)/ac/ttt_core_circuit.h | $(TTT_OBJ_DIR)
	$(CC) $(CPPFLAGS) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

$(TTT_BIN): $(TTT_OBJS) | $(TTT_BIN_DIR)
	$(CC) $(LDFLAGS) $(TTT_OBJS) -o $@

$(POKER_OBJ_DIR):
	mkdir -p $@

$(POKER_BIN_DIR):
	mkdir -p $@

$(RPS_OBJ_DIR):
	mkdir -p $@

$(RPS_BIN_DIR):
	mkdir -p $@

$(HASH_OBJ_DIR):
	mkdir -p $@

$(HASH_BIN_DIR):
	mkdir -p $@

$(TTT_OBJ_DIR):
	mkdir -p $@

$(TTT_BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(POKER_OBJ_DIR) $(POKER_BIN_DIR) $(RPS_OBJ_DIR) $(RPS_BIN_DIR) $(HASH_OBJ_DIR) $(HASH_BIN_DIR) $(TTT_OBJ_DIR) $(TTT_BIN_DIR)

include ../Makefile.config

POKER_NAME := poker_without_revealing_the_cards
POKER_SRC := $(SRC_DIR)/$(POKER_NAME)/poker_without_revealing_the_cards.c
POKER_OBJ_DIR := $(TEMP_DIR)/$(POKER_NAME)
POKER_OBJ := $(POKER_OBJ_DIR)/poker_without_revealing_the_cards.o
POKER_BIN_DIR := $(BUILD_DIR)/$(POKER_NAME)
POKER_BIN := $(POKER_BIN_DIR)/poker_demo
POKER_ARGS ?=

.PHONY: all poker run_poker clean

all: poker

poker: $(POKER_BIN)

run_poker: poker
	$(POKER_BIN) $(POKER_ARGS)

$(POKER_OBJ): $(POKER_SRC) | $(POKER_OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(POKER_BIN): $(POKER_OBJ) | $(POKER_BIN_DIR)
	$(CC) $(LDFLAGS) $< -o $@ $(LDLIBS)

$(POKER_OBJ_DIR):
	mkdir -p $@

$(POKER_BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(POKER_OBJ_DIR) $(POKER_BIN_DIR)

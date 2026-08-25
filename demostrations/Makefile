include ../Makefile.config

.PHONY: demo_poker build_poker demo_rps build_rps demo_hash build_hash \
	demo_ttt build_ttt demo_ttt_bind build_ttt_bind clean

build_poker:
	$(MAKE) -C $(SRC_DIR) poker

demo_poker:
	$(MAKE) -C $(SRC_DIR) run_poker

build_rps:
	$(MAKE) -C $(SRC_DIR) rps

demo_rps:
	$(MAKE) -C $(SRC_DIR) run_rps RPS_ARGS="$(RPS_ARGS)"

build_hash:
	$(MAKE) -C $(SRC_DIR) hash

demo_hash:
	$(MAKE) -C $(SRC_DIR) run_hash

build_ttt:
	$(MAKE) -C $(SRC_DIR) ttt

demo_ttt:
	$(MAKE) -C $(SRC_DIR) run_ttt

build_ttt_bind:
	$(MAKE) -C $(SRC_DIR) ttt_bind

demo_ttt_bind:
	$(MAKE) -C $(SRC_DIR) run_ttt_bind TTT_BIND_ARGS="$(TTT_BIND_ARGS)"

clean:
	$(MAKE) -C $(SRC_DIR) clean

# C Demonstrations

This folder contains C implementations of adversarial cooperation game demos.

Current layout:

- `src/primitives/`: reusable, narrowly scoped cryptographic wrappers.
- `src/circuits/`: dependency-free Boolean circuit representation and
  relation-specific builders.
- `src/protocols/`: participant-local protocol state machines.
- `src/hash_functions/`: fixed public teaching examples for BLAKE2b-256.
- `src/two_oracles_play_rock_paper_scissors/`: RPS teaching driver.
- `src/tic_tac_toe_without_revealing_the_strategy/`: public-policy checker
  teaching driver.
- `src/poker_without_revealing_the_cards/`: earlier centralized poker demo.

Build and run:

```bash
cd /src
make -C demostrations demo_poker
make -C demostrations demo_rps RPS_ARGS='--alice rock --bob scissors'
make -C demostrations demo_hash
make -C demostrations demo_ttt
make -C tests test
```

Output paths:

- Binary: `demostrations/.build/poker_without_revealing_the_cards/poker_demo`
- RPS binary: `demostrations/.build/two_oracles_play_rock_paper_scissors/rps_demo`
- Hash binary: `demostrations/.build/hash_functions/hash_demo`
- Tic-Tac-Toe binary: `demostrations/.build/tic_tac_toe_without_revealing_the_strategy/ttt_demo`
- Temp objects: `.temp/poker_without_revealing_the_cards/`
- RPS temp objects: `.temp/two_oracles_play_rock_paper_scissors/`
- Hash temp objects: `.temp/hash_functions/`
- Tic-Tac-Toe temp objects: `.temp/tic_tac_toe_without_revealing_the_strategy/`

The RPS core is plain C11. Libsodium is the sole external C dependency and is
isolated to hashing, secure random bytes, constant-time comparison, and memory
clearing. The demo is educational rather than production-ready.

The hash wrapper pins unkeyed BLAKE2b with a 32-byte output. It intentionally
does not expose a generic keyed, password, or streaming interface. The
commitment keeps its separately specified canonical incremental encoding.

The Tic-Tac-Toe companion now has three public computation paths: the recursive
diagnostic checker, the fixed-DAG evaluator, and role-specialized XOR/AND
circuits for `Core`. They use only the C11 standard library and link no
cryptographic backend. All three receive the entire policy table in plaintext.
The circuit's witness-typed input range does not hide those bytes, and its
serialization is not a commitment or a proof. Passing any path is not evidence
of strategy secrecy, knowledge, authorship, originality, or future behavior.

# C Implementation and Demonstrations

This folder contains the C sources, public headers, tests, reference vectors,
and demonstration build entry points.

Goal:

- Keep executable C source in `code/src/<chapter_name>/`.
- Keep public headers in `code/include/` and tests and vectors in `code/tests/`.
- Keep canonical manuscript chapters in `doc/content/`.
- Use `code/` for build entry points; build standalone chapter PDFs
  from their canonical manuscript sources.

Entry point:

```bash
cd /workspace
make -C code demo_poker
make -C code demo_rps RPS_ARGS='--alice rock --bob scissors'
make -C code demo_rps RPS_ARGS='--alice rock --bob scissors --selective-abort'
make -C code demo_hash
make -C code demo_ttt
make -C code demo_ttt_bind
make -C code demo_ttt_bind TTT_BIND_ARGS='--reference-vector'
```

The primary Tic-Tac-Toe demonstration checks a fully disclosed deterministic
policy and adds no library dependency. The separate Bind demonstration reuses
libsodium to verify a disclosed opening; its `--reference-vector` mode replays
the same fixed valid policy used by the cross-layer tests. Neither target is a
zero-knowledge or private-evaluation demonstration.

Build outputs:

- `.local/build/` for runnable demo binaries.
- `.local/build/pdf/` for standalone chapter PDFs.
- `.local/scratch/` for intermediary build artifacts.

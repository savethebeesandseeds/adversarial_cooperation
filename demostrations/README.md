# Demonstrations

This folder mirrors the chapter directory structure under `document/content`.

Goal:

- Keep executable C source in `src/<chapter_name>/`.
- Keep canonical manuscript chapters in `document/content/`.
- Use `demostrations/` for build entry points and generated demonstration
  artifacts; avoid duplicating a canonical chapter merely to build it alone.

Entry point:

```bash
cd /src
make -C demostrations demo_poker
make -C demostrations demo_rps RPS_ARGS='--alice rock --bob scissors'
make -C demostrations demo_rps RPS_ARGS='--alice rock --bob scissors --selective-abort'
make -C demostrations demo_hash
make -C demostrations demo_ttt
make -C demostrations demo_ttt_bind
make -C demostrations demo_ttt_bind TTT_BIND_ARGS='--reference-vector'
```

The primary Tic-Tac-Toe demonstration checks a fully disclosed deterministic
policy and adds no library dependency. The separate Bind demonstration reuses
libsodium to verify a disclosed opening; its `--reference-vector` mode replays
the same fixed valid policy used by the cross-layer tests. Neither target is a
zero-knowledge or private-evaluation demonstration.

Build outputs:

- `demostrations/.build/` for runnable demo binaries.
- `.temp/` for intermediary build artifacts.

# Demonstrations

This folder mirrors the chapter directory structure under `document/content`.

Goal:

- Keep executable C source in `src/<chapter_name>/`.
- Keep demonstration `.tex` notes in `demostrations/<chapter_name>/`.
- Grow both trees chapter-by-chapter while preserving naming parity.

Entry point:

```bash
cd /src
make -C demostrations demo_poker
```

Build outputs:

- `demostrations/.build/` for runnable demo binaries.
- `.temp/` for intermediary build artifacts.

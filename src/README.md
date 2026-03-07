# C Demonstrations

This folder contains C implementations of adversarial cooperation game demos.

Current layout:

- `src/poker_without_revealing_the_cards/`: first playable protocol demo.

Build and run:

```bash
cd /src
make -C demostrations demo_poker
```

Output paths:

- Binary: `demostrations/.build/poker_without_revealing_the_cards/poker_demo`
- Temp objects: `.temp/poker_without_revealing_the_cards/`

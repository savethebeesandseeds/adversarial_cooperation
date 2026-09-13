# Adversarial Cooperation

Research into cryptographic cooperation between mutually distrustful parties,
with a short book, a research companion, and educational implementations.

[Read the evolving white paper](doc/Adversarial-Cooperation-White-Paper.pdf)
([editable source](doc/white-paper/README.md))
by Santiago Restrepo · contact@waajacu.com · [waajacu.com](https://waajacu.com/).

## Repository

| Folder | Contents |
|---|---|
| [doc/](doc/README.md) | Books, white paper, research records, earlier writing, and document build tools |
| [code/](code/README.md) | C sources, headers, build files, and tests |
| [code/tests/](code/tests/) | Tests and their reference vectors in `vectors/` |
| [web/](web/README.md) | Browser reader and interactive demonstrations |
| [.local/container/](.local/container/README.md) | Versioned container tooling and environment instructions |

Other `.local/` subfolders hold ignored build outputs, scratch, evidence,
archives, and personal material. See the [folder guide](doc/research/REPOSITORY_LAYOUT.md)
for current paths and preserved older files.

## Working on the project

Use the existing [development container](.local/container/README.md).
From the repository root inside that environment:

```bash
make -C code demo_ttt
make -C code/tests test
bash doc/tools/book.sh -list
AC_RUN_ID=verify-unique bash .local/container/tasks.sh verify
```

`setup.sh` remains at the root because the saved container mounts that exact
file. The other root files provide project navigation, license, agent guidance,
and Git configuration.

## License

The manuscript is licensed under [Creative Commons Attribution-NonCommercial 4.0](LICENSE).
Third-party files retain their own notices and licenses.

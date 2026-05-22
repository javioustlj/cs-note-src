# cs-note-src

A repository for organizing computer science learning notes and source code.

## Directory layout

- `docs/`: notes, terms, abbreviations, and learning summaries.
- `examples/`: small language or tool experiments, such as C++ and Python snippets.
- `topics/`: computer science topics, such as algorithms, data structures, network, and design patterns.
- `problems/`: coding practice and interview problems.
- `scripts/`: helper scripts.

## Build C/C++ examples

```shell
cmake -S . -B build
cmake --build build
```

Generated executables are written to:

```shell
build/bin/
```

Python files under `examples/python/` are regular scripts and are not managed by CMake.

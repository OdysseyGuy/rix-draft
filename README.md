# rix

Welcome to the source code of the _Rix_ micorkernel. This is currently in draft stage and any feature request or bug report is welcome.

## Building

Rix currently uses `make` to build the kernel. See the `Makefile` for details. To build the kernel, run

```sh
make -j$(nproc)
```

### Compiler

Make sure to use the correct cross-compiler for you target architecture. Right now the supported cross-compilers are:

- `x86_64-elf-gcc`

## Code formatting

Rix uses `clang-format` to format the code. All the code styles are defined in the `.clang-format` file.
Note: `clang-format` >= 16 is required.

## License

Rix is licensed under the MIT License. See the `LICENSE` file for details.

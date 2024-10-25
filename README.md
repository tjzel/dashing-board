# Dashing Board

## Installation

The project uses llvm19 toolchain for offline compilation. Make sure you have it installed.

### Configuring CMake (you only need to do it once)

1. Make sure to set your compilers for CMake:

```shell
  export CC=your/path/to/clang
  export CXX=your/path/to/clang++
  cmake .
```

### Compiling

Doesn't have to be done in root.

```shell
  make
```

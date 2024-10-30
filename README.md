# Dashing Board

## Installation

The project uses llvm (at least 18 is recommended) toolchain for offline compilation. Make sure you have it installed.

### Configuring CMake (you only need to do it once)

1. Make sure to set your compilers for CMake:

```shell
  export CC=your/path/to/clang
  export CXX=your/path/to/clang++
  mkdir build
  cd build
  cmake ..
```

### Compiling

```shell
  make
```

The binaries will be outputted to `/bin` directory.

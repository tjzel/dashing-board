# Dashing Board

Dashing Board is a toolchain for real-time automotive eavesdropping of the CAN network. The project uses ESP32 hardware to hook into the CAN network, either directly or via the OBD2 diagnostic interface - there it supports the CAN or KWP2000 implementation of it.

## Repository

Repository consists of four submodules:

- `controller` - platform-agnostic C++ core codebase, which handles all business logic and includes a mock environment to simulate gathering data from a real CAN network.
- `esp32` - ESP32-specific C++ code, which handles the hardware part of the project, linking itself to the `controller`.
- `DashingApp` - showcase React-Native app, which serves as a consumer for the eavesdropped data to act as a Heads Up Display (HUD).
- `server` - simple C++ server, linking itself to the `controller`, for the purpose of mock testing `DashingApp` without any hardware.

## Installation

The project uses llvm (at least 18 is recommended) toolchain for offline compilation. This includes the `clang`, `clang++` compilers and also `CMake` and `Make`. Make sure you have it installed. [How to install LLVM](https://www.llvm.org/docs/GettingStarted.html).

To compile the ESP32 part of the project you need ESP32 compilation toolchain, either installed on your machine or via a Docker container. [How to install ESP32 toolchain](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html), [how to build with ESP32 Docker image](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/tools/idf-docker-image.html).

To run the React Native showcase app, follow the steps in the [Expo app setup guide](https://docs.expo.dev/get-started/set-up-your-environment/).

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

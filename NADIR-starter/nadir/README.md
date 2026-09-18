# NADIR

Navigational & Astronomical Data Instrumentation Renderer.

A terminal-native scientific visualization project focused on real geometry, orbital data, Earth coordinate systems, spacecraft wireframes and network telemetry.

## Requirements

- CMake 3.24+
- C++23 compiler
- Ninja or another CMake generator
- UTF-8 terminal with ANSI support

## Build

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run

```bash
./build/nadir
```

Direct Earth view:

```bash
./build/nadir earth --yaw -28 --pitch 18 --lat -12.093 --lon -45.786
```

Coordinate conversion:

```bash
./build/nadir geo -12.093 -45.786 740
```

## Current state

N0 through the first part of N3 are implemented as a compilable foundation. SGP4, live satellite sources, packet capture, BGP, ephemerides and advanced time systems are intentionally isolated as the next modules rather than faked.

See `docs/ARCHITECTURE.md` and `docs/ROADMAP.md`.

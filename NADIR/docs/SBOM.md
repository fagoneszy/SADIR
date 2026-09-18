# Software Bill of Materials v0.3

## Project

NADIR 0.3.0

## Build-time requirements

```text
CMake >= 3.24
C++23 compiler
Ninja optional but recommended
```

## Runtime requirements

```text
standard C/C++ runtime
curl executable for HTTPS acquisition
ANSI/VT-capable terminal
```

Windows additionally uses operating-system libraries:

```text
iphlpapi
ws2_32
```

Linux uses standard kernel/userspace interfaces for hostname, uptime, memory and network-interface discovery.

## Bundled third-party code

No external SGP4 implementation is bundled in v0.3. The project remains MIT licensed. When the Vallado/CelesTrak SGP4 reference implementation is integrated, its requested attribution will be retained in the project documentation and source distribution.

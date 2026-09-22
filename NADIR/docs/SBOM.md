# Software Bill of Materials v0.3.0

## Project

NADIR 0.3.0 (source inventory)

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

## Bundled third-party source

| Component | Location | Purpose | Attribution / notice |
|---|---|---|---|
| Vallado/CelesTrak SGP4 C++ reference | `third_party/vallado/SGP4.cpp`, `SGP4.h` | GP mean-element propagation | `third_party/vallado/NOTICE.md` |

The NADIR adapter is in `src/orbit/sgp4.cpp`. The reference sources are kept
separate from local code. CelesTrak requests citation of Vallado, Crawford,
Hujsak and Kelso, “Revisiting Spacetrack Report #3”, AIAA 2006-6753,
Revision 2, when using the reference implementation.

## Release inventory

`tools/package-release.ps1` copies this SBOM, the Vallado notice and an
artifact SHA-256 manifest alongside the executable. `SOURCE-MANIFEST.sha256`
binds the release to the verified source-tree inventory. The runtime `curl`
executable and Windows system libraries are external platform dependencies and
are not redistributed by NADIR.

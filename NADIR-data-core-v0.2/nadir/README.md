# NADIR

Navigational & Astronomical Data Instrumentation Renderer.

NADIR is a terminal-first scientific visualization system for Earth, orbital objects, the Solar System, public spacecraft architecture and live data feeds.

The project treats every visual element as data. Positions, vectors, events, routes, measurements and spacecraft metadata are expected to carry a source, timestamp and reference frame whenever the upstream source provides them.

## Current build

Version 0.2 adds the first real data layer on top of the N0-N3 renderer foundation.

Implemented:

- C++23 terminal application
- WGS84 geodetic/ECEF conversion
- Braille framebuffer and Earth wireframe
- TLE parser
- OBJ loader
- NDR header
- UTC/JD/MJD clock
- SHA-256 cache provenance
- source registry
- HTTP acquisition through curl
- source-specific update intervals
- current JPL Horizons vector queries
- planetary physical catalog
- public Starlink V2/V3 subsystem database
- direct feeds for Earth, orbital and space catalogs

Not yet implemented:

- SGP4 propagation
- TEME/ITRF/GCRF transformations
- IERS EOP parser and frame application
- OSM PBF renderer
- DEM terrain renderer
- GIBS imagery renderer
- GRIB2 weather parser
- live RIS WebSocket
- packet capture
- SPICE kernel engine
- generalized spacecraft-document extraction

## Build

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Windows 10/11 and current Linux distributions normally include `curl`. NADIR uses the system curl executable for HTTPS acquisition.

## Commands

```text
nadir
nadir earth --yaw -28 --pitch 18 --lat -12.093 --lon -45.786
nadir geo -12.093 -45.786 740
nadir time
nadir sources
nadir sources earth
nadir sources orbit
nadir sources space
nadir sources structure
nadir sync iers.eop.rapid
nadir sync celestrak.starlink
nadir sync domain:orbit
nadir body info Mars
nadir body live Mars
nadir body live Moon
nadir solar live
nadir craft Starlink V2
nadir craft Starlink V3
```

## Data directories

```text
data/
├── cache/
│   └── <source-id>/
│       ├── <UTC>.<format>
│       ├── <UTC>.meta
│       └── LATEST
└── catalog/
    ├── solar_system.csv
    └── starlink_public_architecture.tsv
```

Every synchronized object receives a metadata sidecar containing the fetch time, URL, byte size, HTTP status and SHA-256 digest.

## Source catalog

`config/sources.tsv` is the current authoritative registry for the data layer. It contains Earth orientation, earthquakes, natural events, space weather, satellite catalogs, Starlink, JPL, exoplanets, spacecraft resources and network feeds.

The source scheduler respects the interval field in the registry and reuses the cached copy until the source becomes due again.

## Scope

NADIR targets all publicly accessible data that can be legally and technically acquired from the machine. It does not invent unavailable internal spacecraft specifications. Classified, proprietary, account-restricted and undocumented data are represented as unavailable or authentication-gated rather than fabricated.

See `docs/DATA_UNIVERSE.md`, `docs/SATELLITE_ARCHITECTURE.md` and `docs/SOURCES.md`.

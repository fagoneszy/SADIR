# NADIR

Navigational & Astronomical Data Instrumentation Renderer.

NADIR is a C++23 terminal-first scientific data and visualization system for Earth, orbital objects, the Solar System, public spacecraft architecture, heliophysics and local machine telemetry.

The design rule is simple: a visual element should represent a real datum, a real calculation or an explicitly marked unknown. Raw acquisitions are cached with timestamps and SHA-256 provenance before normalization.

## v0.3

The 0.3 data-universe build expands the original renderer into a scientific acquisition and normalization core.

Implemented:

- WGS84 geodetic/ECEF conversion
- Braille framebuffer and Earth wireframe
- UTC, TAI, TT, UT1, JD and MJD time state
- leap-second table through the current UTC-TAI offset
- IERS EOP CSV parsing and interpolation
- TEME to ITRF transformation support using UT1, polar motion and LOD
- local ENU observation geometry
- azimuth, elevation, range, range-rate and Doppler helpers
- JSON parser implemented in C++
- CelesTrak OMM JSON ingestion with 64-bit catalog identifiers
- six-digit-plus NORAD/USSF catalog ID support in the OMM path
- JPL Horizons vector parsing and live target queries
- 40 major Solar System targets covering the Sun, planets and major natural satellites
- USGS earthquake GeoJSON normalization
- JPL fireball normalization
- NOAA Kp and solar-wind normalization
- local station inventory and network-interface enumeration
- public spacecraft architecture profiles for Starlink V2/V3, Webb, Hubble, Voyager and ISS
- immutable acquisition cache
- SHA-256 source snapshots
- source metadata and provenance sidecars
- source-specific refresh intervals
- synchronization presets
- 164 registered scientific/public sources
- 113 directly synchronizable public entries

Not claimed as complete yet:

- validated SGP4 propagation
- GCRF/CIRS/TIRS high-precision IAU 2000/2006 frame chain
- AOS/LOS pass solver
- OSM PBF vector database
- DEM terrain mesh
- NASA GIBS imagery renderer
- GRIB2 meteorology decoder
- SPICE kernel engine
- live BGP WebSocket client
- packet capture and process-to-flow attribution
- generalized technical-document extraction for every spacecraft class

NADIR does not substitute approximate orbital propagation for validated SGP4 and label it precise. That stage remains explicitly separate.

## Build

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Windows links against IP Helper and Winsock for station/network discovery. HTTPS acquisition currently uses the system `curl` executable.

## Core commands

```text
nadir
nadir earth --yaw -28 --pitch 18 --lat -12.093 --lon -45.786
nadir geo -12.093 -45.786 740
nadir time
nadir eop live
nadir universe
```

## Data discovery and synchronization

```text
nadir sources stats
nadir sources earth
nadir sources orbit
nadir sources space
nadir sources structure
nadir sources search geomag
nadir source usgs.geomag.api
nadir presets
nadir presets orbit-live
nadir sync preset:core
nadir sync preset:earth-live
nadir sync preset:orbit-live
nadir sync celestrak.starlink
nadir cache celestrak.starlink
```

## Orbital catalog

```text
nadir orbit live starlink
nadir orbit list starlink STARLINK 50
nadir orbit live gps
nadir orbit live stations ISS 10
```

The orbital ingestion path uses OMM-style JSON rather than depending on the legacy fixed-width TLE catalog-number field.

## Solar System

```text
nadir body info Mars
nadir body live Mars
nadir solar live
nadir targets
nadir targets Jupiter
nadir target live Europa
nadir target live Titan
nadir target live 301
```

Live vector queries use JPL Horizons and report position, velocity, range, range-rate and light time relative to Earth for the requested epoch.

## Earth and geospace

```text
nadir seismic hour 20
nadir seismic day 50
nadir spaceweather
nadir fireballs 20
```

The source registry also includes public interfaces for volcanism, geomagnetism, hydrology, ocean observations, weather, terrain, imagery, GPS status, solar events and heliophysics.

## Spacecraft architecture

```text
nadir craft Starlink V2
nadir craft Starlink V3
nadir craft Webb JWST
nadir craft Hubble HST
nadir craft Voyager V1V2
nadir craft ISS Current
```

Only public documented values are included. Unknown proprietary/internal fields remain unknown.

## Local station

```text
nadir station
nadir station interfaces
```

The local station layer is designed to become the observer node for GNSS, SDR, local network telemetry and sensor adapters.

## Data layout

```text
data/
├── cache/
│   └── <source-id>/
│       ├── <UTC>.<format>
│       ├── <UTC>.meta
│       └── LATEST
└── catalog/
    ├── horizons_targets.tsv
    ├── solar_system.csv
    ├── spacecraft_public_architecture.tsv
    └── starlink_public_architecture.tsv
```

## Configuration

```text
config/sources.tsv
config/presets.tsv
```

`config/sources.tsv` is the source-of-truth registry. Entries include authority, domain, coverage, format, authentication class, update interval, licensing label, priority, URL and description.

## Scope

The target is the widest useful collection of legally accessible, publicly documented scientific and technical data that can be acquired and traced to its source. NADIR does not ingest stolen credentials, leaked private datasets, classified material or dark-web dumps. Account-gated official services are represented as gated sources and require the user's own authorized credentials.

See `docs/ARCHITECTURE.md`, `docs/DATA_UNIVERSE.md`, `docs/SOURCE_MATRIX.md`, `docs/VALIDATION.md`, `docs/PUBLIC_DATA_POLICY.md`, `docs/SATELLITE_ARCHITECTURE.md` and `docs/SGP4.md`.

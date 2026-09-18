# NADIR Architecture v0.3

```text
Authoritative/Public Sources
            |
            v
     Source Registry
            |
            +-------------------+
            |                   |
            v                   v
     Sync Presets          Query Adapters
            |                   |
            v                   v
   Acquisition + Rate Control + Auth Boundary
            |
            v
   Raw Immutable Cache + SHA-256 + Metadata
            |
            v
      Parsers / Normalizers
            |
            +-------------------------+
            |            |            |
            v            v            v
        Time Core    Reference Frames Provenance
            |            |            |
            +------------+------------+
                         |
                         v
                Scientific Data Model
                         |
       +-----------------+------------------+
       |                 |                  |
       v                 v                  v
     Earth             Orbit              Space
       |                 |                  |
       +-----------------+------------------+
                         |
                         +-----------> Spacecraft
                         |
                         +-----------> Network/Station
                         |
                         v
                     Scene Graph
                         |
                         v
                    Braille/ANSI
```

## Rules

1. Renderer modules do not perform arbitrary network acquisition.
2. Raw upstream bytes are retained independently of normalized records.
3. Every cache snapshot has a fetch timestamp, source URL, HTTP status, byte count and SHA-256 digest.
4. Reference frames and time scales are explicit whenever a calculation depends on them.
5. Unknown fields are represented as unknown instead of inferred from visual appearance.
6. Authentication-gated sources are not scraped around their access controls.
7. Source-specific polling intervals are enforced by the synchronization engine.
8. Approximate algorithms are not presented as validated precision products.

## Current scientific path

```text
CelesTrak OMM JSON
        |
        v
  OMM normalizer
        |
        v
Orbital elements + epoch
        |
        +--> validated SGP4 target for v0.4
        |
        v
      TEME
        |
        v
IERS EOP + UT1 + polar motion + LOD
        |
        v
      ITRF
        |
        +--> WGS84 geodetic
        +--> local ENU
        +--> azimuth/elevation/range/range-rate
        +--> Doppler
```

The frame module is usable for TEME-to-ITRF state transformations once a validated TEME state vector is supplied. Full IAU 2000/2006 GCRF/CIRS/TIRS work remains a later precision layer.

## Source classes

`none` is public direct access.

`api_key` requires a provider API key.

`account` requires the user's own authorized provider account.

`query` is a query/stream/large-data endpoint requiring a specialized adapter rather than blind periodic download.

`syncable=1` means the generic immutable downloader can safely snapshot the configured URL.

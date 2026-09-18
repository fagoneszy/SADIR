# NADIR Roadmap

## N0-N3 complete foundation

Terminal, CMake, Braille framebuffer, wireframe Earth, WGS84 geodetic conversion and file/model foundations.

## Data Core 0.3

Implemented before the renderer expansion:

- 164-source registry
- source authority/access/update metadata
- immutable SHA-256 cache
- synchronization presets
- UTC/TAI/TT/UT1 core
- IERS EOP parser/interpolator
- TEME-to-ITRF utility layer
- ENU, range/range-rate and Doppler helpers
- JSON parser
- OMM ingestion with 64-bit catalog IDs
- JPL Horizons vector ingestion
- 40-target Solar System catalog
- earthquake/fireball/space-weather normalizers
- local station/interface discovery
- initial public spacecraft architecture library

## N4 Earth geometry and mapping

- Natural Earth vector ingestion
- map projections
- coastline/admin spatial index
- OSM PBF decoder/index
- ETOPO/Copernicus DEM tile model
- terrain normals and mesh generation
- NASA GIBS tile adapter

## N5 validated orbital propagation

- Vallado/CelesTrak SGP4 integration
- reference verification suite
- OMM conversion path
- near-Earth and deep-space handling
- epoch parsing and propagation clock

## N6 orbital scene

- satellite positions
- Starlink/OneWeb/Kuiper constellation layers
- object filters
- spacecraft-class linkage
- orbit trails
- stale-element warnings

## N7 observer geometry

- ground tracks
- AOS/LOS solver
- maximum elevation
- azimuth/elevation
- range/range-rate
- Doppler
- visibility/eclipses

## N8 precision frames/time

- full leap-second data refresh path
- IAU 2000/2006 precession-nutation
- GCRF/CIRS/TIRS/ITRF
- CIO/ERA path
- SOFA-compatible verification

## N9 Solar System and SPICE

- Horizons generalized target adapter
- natural satellite physical catalog
- small-body adapter
- NAIF SPICE kernel loader
- spacecraft attitude/instrument frame geometry
- DSN state layer

## N10 spacecraft engineering

- OBJ/STL/GLB renderer
- NASA 3D acquisition
- architecture evidence store
- subsystem labels
- NTRS/PDS document indexing
- DISCOS authorized adapter
- class-to-instance mapping

## N11 dynamic Earth

- GFS/GEFS GRIB2
- NWS alerts
- USGS water
- volcano notices
- geomagnetic observatories
- FIRMS fire points
- ocean buoy/Argo adapters
- SWPC/DONKI/CDAWeb layers

## N12 local systems/network

- route and neighbor tables
- process/socket attribution
- Npcap/libpcap
- 5-tuple flow table
- local topology
- GNSS/serial/SDR adapter boundary

## N13 Internet routing

- RIPE RIS Live WebSocket
- RouteViews ingestion
- RPKI validation state
- ASN/prefix graph
- time-window replay

## N14 record/replay

- NDR indexed container
- normalized event stream
- raw-source references
- deterministic replay
- timeline seeking

## N15 renderer

- depth buffer
- depth cueing
- quaternion camera
- clipping
- CRT persistence
- phosphor decay
- AVX2/SIMD profiling and integration

## N16 verification

- SGP4 reference report
- Horizons/SPICE comparisons
- frame-conversion reference cases
- deterministic regression vectors
- source schema regression suite

## N17 source platform

- plugin/source adapter ABI
- credential vault abstraction
- per-provider quota policies
- schema migration
- health/latency telemetry
- resumable large-object acquisition

## N18 operationalization

- ADRs
- data dictionaries
- operator manual
- reproducible builds
- signed releases
- SBOM
- supply-chain verification
- benchmark suite

# Source Registry v0.3

The machine-readable registry is `config/sources.tsv`. v0.3 contains 164 definitions from 51 authorities/providers.

## High-value authorities and services

### Time, geodesy and Earth reference

- IERS EOP and Bulletins
- NIST time dissemination reference
- NGA WGS84 and EGM2008
- NOAA WMM2025
- NOAA CORS
- International GNSS Service

### Earth observation and hazards

- NASA EOSDIS CMR and GIBS
- NASA EONET
- NASA FIRMS
- NASA POWER
- USGS Earthquake Hazards Program
- USGS Volcano Hazards Program HANS/VONA
- USGS Geomagnetism web service
- USGS Water APIs
- NOAA NCEP NOMADS GFS/GEFS
- National Weather Service API
- NOAA NDBC
- NOAA CO-OPS
- NOAA ERDDAP installations
- NOAA SWPC
- NOAA Aviation Weather Center
- National Hurricane Center GIS
- EPA AirNow
- OpenStreetMap
- Natural Earth
- Copernicus Data Space

### Orbital catalog and GNSS

- CelesTrak GP/OMM and SupGP
- Space-Track account-gated services
- USCG Navigation Center GPS status/NANU/almanacs

### Solar System and small bodies

- JPL Horizons
- JPL Small-Body Database
- JPL Close Approach Data
- JPL Sentry
- JPL Scout
- JPL NHATS
- JPL Fireball API
- JPL small-body satellite, radar and mission-design services
- Minor Planet Center

### Heliophysics and astronomy

- NOAA SWPC
- NASA DONKI
- NASA CDAWeb/HAPI
- NASA OMNIWeb
- NASA HEASARC
- NASA MAST
- NASA Exoplanet Archive
- NASA PDS
- NAIF SPICE
- NASA DSN Now
- ESA Gaia TAP
- ESO TAP

### Spacecraft engineering

- NASA 3D Resources
- NASA Technical Reports Server
- NASA PDS
- NAIF mission kernels
- ESA DISCOS
- ESA Space Debris User Portal documents
- public Starlink engineering pages
- FCC public filings
- ITU references

### Network

- RIPE RIS Live
- RouteViews
- RPKI integration target

## Access semantics

`none` means a public endpoint can be accessed without a credential.

`api_key` means a provider-issued API key is required.

`account` means the user's own authorized account is required.

`earthdata` means NASA Earthdata authentication is required for the actual data product.

`query` means the configured entry is a dynamic query, stream, large-data service or specialized protocol that needs an adapter.

`syncable=1` means the generic immutable downloader may snapshot the configured URL while respecting its refresh interval.

## Provenance

Every generic synchronized snapshot receives:

```text
source id
domain
format
authority
coverage
license label
fetch time
HTTP status
byte count
SHA-256
source URL
```

See `SOURCE_MATRIX.md` for the full inventory.

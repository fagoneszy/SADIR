# NADIR Data Universe v0.3

The 0.3 registry contains 164 source definitions across time, Earth, orbit, space, spacecraft structure and network domains. It is deliberately a registry rather than a giant bundled dataset: NADIR records where authoritative data lives, how it may be accessed, how often it should be refreshed and whether credentials are required.

## Time and reference frames

Sources include IERS Earth Orientation Parameters and Bulletin C, NIST time dissemination references, WGS84, EGM2008, NOAA WMM2025, NOAA CORS and IGS products.

Normalized state includes:

```text
UTC
TAI
TT
UT1
JD
MJD
TAI-UTC
DUT1
xp
yp
LOD
dX
dY
```

## Earth

Static/reference layers:

```text
WGS84
EGM2008
WMM2025
Natural Earth
OpenStreetMap
ETOPO 2022
Copernicus DEM
SRTM
Landsat discovery
NASA CMR
NASA GIBS
NOAA CORS
IGS
```

Dynamic layers:

```text
USGS earthquakes
USGS volcano HANS/VONA
USGS geomagnetic observatories
USGS water
NASA EONET
NASA FIRMS
NOAA GFS/GEFS
NWS alerts
NDBC stations
NOAA CO-OPS
NOAA ERDDAP ocean datasets
NOAA SWPC
Aviation Weather
NHC GIS
EPA AirNow
NASA POWER
JPL fireballs
```

## Orbital environment

The primary machine-readable catalog path is OMM-compatible JSON rather than legacy TLE-only ingestion.

Registered CelesTrak groups include active objects, stations, Starlink, OneWeb, Kuiper, GPS, GLONASS, Galileo, BeiDou, SBAS, GEO, weather, science, geodetic, engineering, amateur, CubeSats, resource satellites, SAR, SARSAT, TDRSS, Planet, Spire, Iridium NEXT, Orbcomm, Globalstar and multiple debris groups.

Additional official/gated references include Space-Track GP/SATCAT/CDM and USCG Navigation Center GPS constellation, NANU and almanac products.

The OMM parser stores NORAD/USSF catalog IDs in 64-bit integers.

## Solar System

JPL Horizons is the live vector-ephemeris layer. v0.3 ships a 40-target catalog including:

```text
Sun
8 planets
Earth's Moon
Phobos and Deimos
Io, Europa, Ganymede, Callisto, Amalthea
Mimas, Enceladus, Tethys, Dione, Rhea, Titan, Hyperion, Iapetus, Phoebe
Ariel, Umbriel, Titania, Oberon, Miranda
Triton, Nereid, Proteus
Pluto, Charon, Nix, Hydra, Kerberos, Styx
```

The broader small-body layer registers JPL SBDB, close-approach data, Sentry, Scout, NHATS, small-body satellites, radar observations and mission-design services, plus Minor Planet Center observations.

## Heliophysics and astronomy

Registered services include:

```text
NOAA SWPC
NASA DONKI
NASA CDAWeb/HAPI
NASA OMNIWeb
NASA HEASARC
NASA MAST
NASA Exoplanet Archive
NASA PDS
NAIF SPICE
NASA DSN Now
ESA Gaia TAP
ESO TAP
```

NASA DONKI coverage includes CME, CME analysis, geomagnetic storms, interplanetary shocks, solar flares, solar energetic particles, high-speed streams, notifications and WSA-Enlil simulation products.

## Spacecraft and infrastructure

The structure domain combines operator documentation, NASA technical repositories and public regulatory/agency sources:

```text
NASA 3D Resources
NASA NTRS
NASA PDS
NAIF SPICE
Starlink public engineering material
FCC public filings
ESA DISCOS
ESA Space Debris engineering documents
ITU references
```

DISCOS is registered as account-gated because its terms and API access are provider-controlled.

## Network

The global routing layer registers RIPE RIS Live, RouteViews and RPKI integration targets. The local station layer currently enumerates machine/network-interface state and will later connect packet-flow capture and process attribution.

## Quality principle

A large source count is not itself scientific quality. NADIR keeps acquisition, provenance, frame/time normalization and validation separate so contradictory or stale upstream data can be identified instead of silently merged.

# NADIR Data Universe

The target is a unified local scientific datastore built from public authoritative sources.

## Earth

Core geometry uses WGS84. Earth orientation is sourced from IERS. Surface mapping is intended to combine Natural Earth for global vector geometry, OpenStreetMap for detailed vector mapping, Copernicus DEM for terrain and NASA GIBS for orbital imagery.

Dynamic layers include USGS earthquakes, NASA EONET events, NASA FIRMS fire detections, NOAA GFS weather, NOAA SWPC space weather and auroral products.

Planned normalized domains:

```text
earth.reference
 earth.wgs84
 earth.eop
 earth.coastline
 earth.admin
 earth.roads
 earth.buildings
 earth.terrain
 earth.imagery
 earth.weather
 earth.seismic
 earth.fire
 earth.aurora
 earth.space_weather
```

## Orbital environment

Primary machine-readable orbit format is OMM-compatible JSON/CSV rather than legacy TLE.

The orbital catalog will combine:

```text
CelesTrak GP
CelesTrak SupGP
Space-Track when credentials are supplied
owner/operator ephemerides where public
NASA OEM/mission ephemerides where public
SPICE kernels for supported missions
```

Every object will eventually normalize into:

```text
catalog identity
international designator
operator
object class
launch metadata
epoch
orbital elements
state vectors
reference frame
source
uncertainty or source age
spacecraft class
architecture profile
```

## Solar System

JPL Horizons is the live ephemeris layer. NADIR uses it for geometric state vectors, range, range-rate and light time relative to Earth.

JPL physical parameter tables provide canonical body properties. JPL planetary-satellite tables provide natural-moon physical parameters and associated high-precision ephemeris families.

JPL SBDB and the Minor Planet Center cover asteroids and comets. NASA PDS and NAIF SPICE provide mission science archives, trajectories, orientation, instrument frames and geometry when available.

## Exoplanets

NASA Exoplanet Archive TAP is the primary catalog adapter. The full `pscomppars` dataset is registered as a synchronizable source.

## Spacecraft structure

Architecture data is class-based and evidence-based. A satellite instance may reference a spacecraft class, generation or bus. Public fields can include:

```text
structure
mass
dimensions
power generation
battery
propulsion
attitude determination and control
flight computer
communications
antennas
optical links
payloads
thermal control
navigation sensors
radiators
solar arrays
bus family
instrument geometry
3D/CAD model
technical-document references
```

Unknown values remain unknown.

## Network

The global layer will ingest RIPE RIS Live BGP events. The local layer will use OS-native network tables and packet capture when implemented.

## Data quality levels

```text
A  authoritative operator/agency data
B  authoritative derived service
C  documented secondary technical source
D  inferred association
U  unknown/unverified
```

The renderer should expose data quality and source age rather than hiding them.

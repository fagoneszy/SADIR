# Source Registry

The machine-readable source registry is `config/sources.tsv`.

Primary sources currently represented:

- NGA WGS84: https://earth-info.nga.mil/
- IERS EOP: https://datacenter.iers.org/eop.php
- USGS Earthquake Hazards Program: https://earthquake.usgs.gov/earthquakes/feed/
- NASA EONET: https://eonet.gsfc.nasa.gov/docs/v3
- NASA GIBS: https://gibs.earthdata.nasa.gov/
- NASA FIRMS: https://firms.modaps.eosdis.nasa.gov/
- NOAA SWPC: https://services.swpc.noaa.gov/
- NOAA/NCEP NOMADS GFS: https://nomads.ncep.noaa.gov/
- OpenStreetMap replication: https://planet.openstreetmap.org/replication/
- Natural Earth: https://www.naturalearthdata.com/
- Copernicus Data Space: https://documentation.dataspace.copernicus.eu/
- CelesTrak: https://celestrak.org/
- Space-Track: https://www.space-track.org/
- JPL Horizons: https://ssd.jpl.nasa.gov/api/horizons.api
- JPL planetary parameters: https://ssd.jpl.nasa.gov/planets/phys_par.html
- JPL planetary satellite parameters: https://ssd.jpl.nasa.gov/sats/phys_par/
- JPL Small-Body Database API: https://ssd-api.jpl.nasa.gov/doc/sbdb.html
- Minor Planet Center: https://docs.minorplanetcenter.net/
- NASA Exoplanet Archive: https://exoplanetarchive.ipac.caltech.edu/
- NASA Planetary Data System: https://pds.nasa.gov/
- NAIF SPICE: https://naif.jpl.nasa.gov/
- NASA 3D Resources: https://www.nasa.gov/3d-resources/
- NASA Technical Reports Server: https://ntrs.nasa.gov/
- Starlink technology: https://starlink.com/technology
- Starlink V3: https://starlink.com/updates/starlink-version-3-satellites
- RIPE RIS Live: https://ris-live.ripe.net/

## Access classes

`none` means the endpoint is public without credentials.

`account` means the provider requires an account or authenticated access.

`map_key` means an API key must be supplied by the user.

`syncable=0` means the entry is a query service, stream, large dataset or authenticated endpoint and requires a specialized adapter instead of blind periodic downloading.

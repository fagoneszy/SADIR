# Source Matrix

Total registered sources: **164**.

The machine-readable authority is `config/sources.tsv`. This document is generated as a human-readable inventory.

## Earth (58)

| ID | Authority | Coverage | Access | Sync |
|---|---|---|---|---:|
| `aviationweather.metars` | NOAA Aviation Weather Center | Global aviation observations | none | yes |
| `aviationweather.stations` | NOAA Aviation Weather Center | Aviation stations | none | yes |
| `aviationweather.tafs` | NOAA Aviation Weather Center | Global aviation forecasts | none | yes |
| `copernicus.dem` | Copernicus Data Space | Elevation | account | adapter |
| `epa.airnow` | EPA AirNow | Air quality | api_key | adapter |
| `esa.swarm.products` | ESA | Earth magnetic field and upper atmosphere | none | yes |
| `igs.products` | International GNSS Service | Precise GNSS products | none | yes |
| `nasa.cmr.collections` | NASA EOSDIS | Earth science catalog | none | yes |
| `nasa.cmr.granules` | NASA EOSDIS | Earth science granules | none | adapter |
| `nasa.eonet.open` | NASA GSFC | Natural hazards | none | yes |
| `nasa.firms.area` | NASA LANCE FIRMS | Wildfire hotspots | map_key | adapter |
| `nasa.firms.availability` | NASA LANCE FIRMS | Wildfire source availability | map_key | adapter |
| `nasa.gibs.capabilities` | NASA EOSDIS | Earth imagery | none | yes |
| `nasa.power` | NASA POWER | Meteorology and solar energy | none | adapter |
| `nasa.srtm` | NASA EOSDIS | Elevation | earthdata | adapter |
| `naturalearth.catalog` | Natural Earth | Global cartography | none | yes |
| `ndbc.active.stations` | NOAA NDBC | Ocean and weather stations | none | yes |
| `ndbc.realtime.index` | NOAA NDBC | Ocean and weather observations | none | yes |
| `nga.egm2008` | NGA | Gravity and geoid | none | yes |
| `nga.wgs84` | NGA | Geodesy | none | yes |
| `nhc.gis` | NOAA NHC | Tropical cyclones | none | yes |
| `noaa.coops.api` | NOAA CO-OPS | Tides currents water levels | none | adapter |
| `noaa.coops.metadata` | NOAA CO-OPS | Coastal station metadata | none | adapter |
| `noaa.cors` | NOAA NGS | GNSS reference stations | none | yes |
| `noaa.cors.data` | NOAA NGS | GNSS observations and navigation | none | yes |
| `noaa.erddap.coops` | NOAA CO-OPS | Coastal oceanographic datasets | none | yes |
| `noaa.erddap.ncei` | NOAA NCEI | Oceanographic and environmental datasets | none | yes |
| `noaa.erddap.osmc` | NOAA GOMO OSMC | Global ocean observing data | none | yes |
| `noaa.erddap.pmel` | NOAA PMEL | Ocean climate and buoy datasets | none | yes |
| `noaa.etopo2022` | NOAA NCEI | Topography and bathymetry | none | yes |
| `noaa.gefs.0p25` | NOAA NCEP | Global ensemble weather | none | adapter |
| `noaa.gfs.0p25` | NOAA NCEP | Global weather model | none | adapter |
| `noaa.swpc.alerts` | NOAA SWPC | Space weather alerts | none | yes |
| `noaa.swpc.aurora` | NOAA SWPC | Aurora | none | yes |
| `noaa.swpc.dst` | NOAA SWPC | Geomagnetic storms | none | yes |
| `noaa.swpc.f107` | NOAA SWPC | Solar radio flux | none | yes |
| `noaa.swpc.kp` | NOAA SWPC | Geomagnetic activity | none | yes |
| `noaa.swpc.scales` | NOAA SWPC | Space weather scales | none | yes |
| `noaa.swpc.solarwind.mag` | NOAA SWPC | Interplanetary magnetic field | none | yes |
| `noaa.swpc.solarwind.plasma` | NOAA SWPC | Solar wind | none | yes |
| `noaa.wmm.coefficients` | NOAA NCEI | Geomagnetism | none | yes |
| `noaa.wmm2025` | NOAA NCEI | Geomagnetism | none | yes |
| `nws.alerts.active` | NOAA NWS | Weather alerts | none | yes |
| `nws.api` | NOAA NWS | Forecasts and observations | none | adapter |
| `osm.planet` | OpenStreetMap | Global map database | none | adapter |
| `osm.replication.minute` | OpenStreetMap | Global map change stream | none | yes |
| `usgs.earthquake.catalog` | USGS | Seismic catalog | none | adapter |
| `usgs.earthquakes.day` | USGS | Seismic hazards | none | yes |
| `usgs.earthquakes.hour` | USGS | Seismic hazards | none | yes |
| `usgs.earthquakes.week` | USGS | Seismic hazards | none | yes |
| `usgs.geomag.api` | USGS | Geomagnetic observatories | query | adapter |
| `usgs.landsat.access` | USGS | Earth observation imagery | varies | adapter |
| `usgs.volcano.hans.api` | USGS Volcano Hazards Program | Volcano hazard search | query | adapter |
| `usgs.volcano.hans.recent` | USGS Volcano Hazards Program | Volcano notices | none | yes |
| `usgs.volcano.vona` | USGS Volcano Hazards Program | Volcanic activity notices for aviation | none | yes |
| `usgs.water` | USGS | Hydrology | optional_key | adapter |
| `usgs.water.instantaneous` | USGS | Hydrology near-real-time | query | adapter |
| `usgs.water.nextgen` | USGS | Hydrology next-generation API | query | adapter |

## Network (3)

| ID | Authority | Coverage | Access | Sync |
|---|---|---|---|---:|
| `ripe.ris.live` | RIPE NCC | Global BGP | none | adapter |
| `routeviews` | Route Views | Global BGP | none | yes |
| `rpki.routinator` | NLnet Labs | RPKI validation | local | adapter |

## Orbit (49)

| ID | Authority | Coverage | Access | Sync |
|---|---|---|---|---:|
| `celestrak.active` | CelesTrak | Earth orbit | none | yes |
| `celestrak.amateur` | CelesTrak | Earth orbit | none | yes |
| `celestrak.argos` | CelesTrak | Earth orbit | none | yes |
| `celestrak.beidou` | CelesTrak | Earth orbit | none | yes |
| `celestrak.cosmos2251.debris` | CelesTrak | Earth orbit | none | yes |
| `celestrak.cubesat` | CelesTrak | Earth orbit | none | yes |
| `celestrak.education` | CelesTrak | Earth orbit | none | yes |
| `celestrak.engineering` | CelesTrak | Earth orbit | none | yes |
| `celestrak.eutelsat` | CelesTrak | Earth orbit | none | yes |
| `celestrak.fengyun.debris` | CelesTrak | Earth orbit | none | yes |
| `celestrak.galileo` | CelesTrak | Earth orbit | none | yes |
| `celestrak.geo` | CelesTrak | Earth orbit | none | yes |
| `celestrak.geodetic` | CelesTrak | Earth orbit | none | yes |
| `celestrak.globalstar` | CelesTrak | Earth orbit | none | yes |
| `celestrak.glonass` | CelesTrak | Earth orbit | none | yes |
| `celestrak.gnss` | CelesTrak | Earth orbit | none | yes |
| `celestrak.gps` | CelesTrak | Earth orbit | none | yes |
| `celestrak.groups` | CelesTrak | Orbital catalog groups | none | yes |
| `celestrak.intelsat` | CelesTrak | Earth orbit | none | yes |
| `celestrak.iridium.next` | CelesTrak | Earth orbit | none | yes |
| `celestrak.iridium33.debris` | CelesTrak | Earth orbit | none | yes |
| `celestrak.kuiper` | CelesTrak | Earth orbit | none | yes |
| `celestrak.last30` | CelesTrak | Earth orbit | none | yes |
| `celestrak.military` | CelesTrak | Earth orbit | none | yes |
| `celestrak.oneweb` | CelesTrak | Earth orbit | none | yes |
| `celestrak.orbcomm` | CelesTrak | Earth orbit | none | yes |
| `celestrak.planet` | CelesTrak | Earth orbit | none | yes |
| `celestrak.radar` | CelesTrak | Earth orbit | none | yes |
| `celestrak.resource` | CelesTrak | Earth orbit | none | yes |
| `celestrak.sar` | CelesTrak | Earth orbit | none | yes |
| `celestrak.sarsat` | CelesTrak | Earth orbit | none | yes |
| `celestrak.satnogs` | CelesTrak | Earth orbit | none | yes |
| `celestrak.sbas` | CelesTrak | Earth orbit | none | yes |
| `celestrak.science` | CelesTrak | Earth orbit | none | yes |
| `celestrak.ses` | CelesTrak | Earth orbit | none | yes |
| `celestrak.spire` | CelesTrak | Earth orbit | none | yes |
| `celestrak.starlink` | CelesTrak | Earth orbit | none | yes |
| `celestrak.stations` | CelesTrak | Earth orbit | none | yes |
| `celestrak.supgp.starlink` | CelesTrak | Operator-derived Starlink ephemerides | none | adapter |
| `celestrak.tdrss` | CelesTrak | Earth orbit | none | yes |
| `celestrak.telesat` | CelesTrak | Earth orbit | none | yes |
| `celestrak.visual` | CelesTrak | Earth orbit | none | yes |
| `celestrak.weather` | CelesTrak | Earth orbit | none | yes |
| `spacetrack.cdm` | USSF 18 SDS Space-Track | Conjunction data messages | account | adapter |
| `spacetrack.gp` | USSF 18 SDS Space-Track | Earth orbit catalog | account | adapter |
| `spacetrack.satcat` | USSF 18 SDS Space-Track | Satellite catalog metadata | account | adapter |
| `uscg.gps.almanacs` | USCG Navigation Center | GPS NANU almanac operations advisories | none | yes |
| `uscg.gps.archives` | USCG Navigation Center | GPS programmatic archives | query | adapter |
| `uscg.gps.constellation` | USCG Navigation Center | GPS operational constellation | none | yes |

## Space (39)

| ID | Authority | Coverage | Access | Sync |
|---|---|---|---|---:|
| `esa.gaia.tap` | ESA | Milky Way astrometry and stellar catalog | query | adapter |
| `eso.tap.catalogs` | ESO | Astronomical catalogs | query | adapter |
| `eso.tap.observations` | ESO | Astronomical observations | query | adapter |
| `jpl.closeapproach` | NASA JPL CNEOS | Small-body close approaches | none | adapter |
| `jpl.fireballs` | NASA JPL CNEOS | Atmospheric impact events | none | yes |
| `jpl.horizons` | NASA JPL SSD | Solar system ephemerides | none | adapter |
| `jpl.horizons.lookup` | NASA JPL SSD | Solar system object lookup | none | adapter |
| `jpl.missiondesign` | NASA JPL SSD | Mission design | none | adapter |
| `jpl.nhats` | NASA JPL CNEOS | Accessible NEO trajectories | none | yes |
| `jpl.planet.physical` | NASA JPL SSD | Planet physical parameters | none | yes |
| `jpl.satellite.physical` | NASA JPL SSD | Natural satellite physical parameters | none | yes |
| `jpl.sb.radar` | NASA JPL SSD | Radar astrometry | none | yes |
| `jpl.sb.satellites` | NASA JPL SSD | Moons of asteroids and comets | none | yes |
| `jpl.sbdb` | NASA JPL SSD | Asteroids and comets | none | adapter |
| `jpl.sbdb.query` | NASA JPL SSD | Small-body catalog | none | yes |
| `jpl.scout` | NASA JPL CNEOS | New NEO candidates | none | yes |
| `jpl.sentry` | NASA JPL CNEOS | Impact risk | none | yes |
| `mpc.observations` | Minor Planet Center | Small-body observations | none | adapter |
| `naif.generic` | NASA JPL NAIF | SPICE kernels | none | yes |
| `naif.missions` | NASA JPL NAIF | Mission SPICE kernels | none | yes |
| `nasa.cdaweb.cdas` | NASA GSFC SPDF | Heliophysics data and metadata | query | adapter |
| `nasa.cdaweb.hapi` | NASA GSFC SPDF | Heliophysics datasets | none | yes |
| `nasa.donki.cme` | NASA CCMC | Coronal mass ejections | api_key | adapter |
| `nasa.donki.cme.analysis` | NASA CCMC | Coronal mass ejection analyses | api_key | adapter |
| `nasa.donki.enlil` | NASA CCMC | WSA-Enlil heliospheric simulations | api_key | adapter |
| `nasa.donki.flr` | NASA CCMC | Solar flares | api_key | adapter |
| `nasa.donki.gst` | NASA CCMC | Geomagnetic storms | api_key | adapter |
| `nasa.donki.hss` | NASA CCMC | High speed solar wind streams | api_key | adapter |
| `nasa.donki.ips` | NASA CCMC | Interplanetary shocks | api_key | adapter |
| `nasa.donki.notifications` | NASA CCMC | Space weather notifications | api_key | adapter |
| `nasa.donki.sep` | NASA CCMC | Solar energetic particle events | api_key | adapter |
| `nasa.dsn.now` | NASA JPL | Deep Space Network live state | none | yes |
| `nasa.exoplanets` | NASA Exoplanet Archive | Exoplanets | none | yes |
| `nasa.heasarc` | NASA GSFC HEASARC | High-energy astrophysics catalogs and missions | none | yes |
| `nasa.heasarc.api` | NASA GSFC HEASARC | Astronomy catalogs images spectra | query | adapter |
| `nasa.mast` | NASA STScI MAST | Optical ultraviolet near-infrared astronomy | none | yes |
| `nasa.omniweb` | NASA GSFC SPDF | Heliospheric plasma magnetic field and trajectories | none | yes |
| `nasa.pds` | NASA PDS | Planetary science archive | none | yes |
| `usgs.astrogeology` | USGS Astrogeology | Planetary cartography | none | yes |

## Structure (11)

| ID | Authority | Coverage | Access | Sync |
|---|---|---|---|---:|
| `esa.discos` | ESA Space Debris Office | Unclassified space objects launch and physical properties | account | adapter |
| `esa.space.debris.docs` | ESA Space Debris Office | Space debris engineering standards and guidance | none | yes |
| `fcc.ecfs` | FCC | Satellite regulatory filings | none | adapter |
| `fcc.starlink.gen2` | FCC | Starlink authorization and shells | none | adapter |
| `itu.spaceexplorer` | ITU | Satellite network filings | none | adapter |
| `nasa.3d` | NASA | 3D spacecraft models | none | yes |
| `nasa.ntrs.propulsion` | NASA STI | Propulsion and power | none | yes |
| `nasa.ntrs.remote.sensing` | NASA STI | Earth observation instruments | none | yes |
| `nasa.ntrs.spacecraft` | NASA STI | Spacecraft engineering | none | yes |
| `starlink.technology` | SpaceX Starlink | Starlink spacecraft technology | none | yes |
| `starlink.v3` | SpaceX Starlink | Starlink V3 architecture | none | yes |

## Time (4)

| ID | Authority | Coverage | Access | Sync |
|---|---|---|---|---:|
| `iers.bulletin.c72` | IERS | Leap seconds | none | yes |
| `iers.bulletins` | IERS | Time and Earth orientation | none | yes |
| `iers.eop.rapid` | IERS | Earth orientation | none | yes |
| `nist.internet.time` | NIST | UTC time dissemination | none | yes |


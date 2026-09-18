# Data Dictionary v0.3

## Source

```text
id
unique stable source identifier

domain
time | earth | orbit | space | structure | network

format
expected upstream representation

interval_seconds
minimum generic resynchronization interval

auth
access class

syncable
whether the generic snapshot downloader may acquire the configured URL

authority
provider responsible for the upstream data

coverage
semantic coverage of the source

license
registry label for usage/public-data terms

priority
source preference metadata; not a numerical statement of scientific certainty

url
configured endpoint or landing page

description
human-readable purpose
```

## OMM orbital record

```text
object_name
object_id
epoch
classification_type
center_name
ref_frame
time_system
mean_element_theory
norad_cat_id uint64
ephemeris_type
element_set_no
rev_at_epoch
mean_motion_rev_day
eccentricity
inclination_deg
raan_deg
arg_pericenter_deg
mean_anomaly_deg
bstar
mean_motion_dot
mean_motion_ddot
```

## EOP record

```text
mjd
xp_arcsec
yp_arcsec
dut1_s
lod_ms
dx_mas
dy_mas
prediction
```

## Earthquake record

```text
id
place
url
longitude_deg
latitude_deg
depth_km
magnitude
time_ms
updated_ms
```

## Time state

```text
unix_ns
jd_utc
mjd_utc
tai_minus_utc_s
jd_tai
jd_tt
dut1_s
jd_ut1
```

## Solar target

```text
name
horizons_id
type
primary
```

## Spacecraft architecture component

```text
subsystem
name
value
source
```

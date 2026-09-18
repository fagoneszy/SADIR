# SGP4 Integration Target

NADIR v0.3 intentionally stops at normalized OMM elements plus reference-frame utilities. It does not include a home-grown approximate propagator labeled SGP4.

The target baseline is the Vallado/CelesTrak implementation associated with AIAA 2006-6753, `Revisiting Spacetrack Report #3`, including its verification cases.

CelesTrak states that the associated code may be used for personal or commercial purposes and requests source attribution. The reference implementation produces position and velocity in TEME.

The integration path is:

```text
OMM JSON
  |
  v
normalized GP elements
  |
  v
validated SGP4
  |
  v
TEME r/v
  |
  v
IERS EOP + UT1
  |
  v
ITRF r/v
  |
  +--> WGS84 subpoint
  +--> local ENU
  +--> az/el/range/range-rate
  +--> Doppler
  +--> ground track
  +--> AOS/LOS solver
```

The OMM path is primary because modern catalog identifiers are no longer safely representable by the historical five-digit TLE catalog-number field. Legacy TLE remains an import format.

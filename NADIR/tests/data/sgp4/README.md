# SGP4 Verification Data

## Source
Vallado, D. A., Crawford, P., Hujsak, R., & Kelso, T. S. (2006).
"Revisiting Spacetrack Report #3."
AIAA 2006-6753. (Commonly referred to as "SGP4 Verification" paper.)

## Files
- `verification.tle` — Two-line element sets for test satellites
- `verification_expected.tsv` — Reference TEME position/velocity at specified minutes since epoch

## Test Case 1: ISS (ZARYA), NORAD 25544
TLE Epoch: 2008-09-20 12:25:39.004 UTC
JD UTC: 2454726.51782528

Reference values computed with Vallado's reference SGP4 implementation (C++).

## Verification Criteria
Position error < 100 m at epoch
Velocity error < 1 mm/s at epoch
Error growth over 2 orbits < 1 km position, < 1 cm/s velocity

## Usage
Tests should parse the TLE, convert to OmmRecord, call `propagate_sgp4()`, and compare against expected values using Euclidean distance for position and velocity vectors.
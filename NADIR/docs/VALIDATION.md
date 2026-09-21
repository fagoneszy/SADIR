# Scientific validation

## TEME to ITRF

`nadir_frames_tests` includes the independent reference case from Appendix C
of Vallado, Crawford, Hujsak and Kelso, *Revisiting Spacetrack Report #3*,
AIAA 2006-6753 Rev. 2: 2004-04-06T07:51:28.386 UTC, with UT1-UTC, `xp` and
`yp` supplied by the publication. The test compares position to 5 cm and
velocity to 1 mm/s after TEME → PEF → ITRF.

This validates the implemented Vallado TEME/PEF/ITRF convention only. It is
not an assertion of full IAU 2000/2006 GCRF/CIRS/TIRS conformance.

Source: https://celestrak.org/publications/aiaa/2006-6753/AIAA-2006-6753-Rev2.pdf

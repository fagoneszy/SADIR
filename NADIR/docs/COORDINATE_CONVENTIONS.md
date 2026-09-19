# Coordinate Conventions

Frozen for 0.4A and beyond. Never mix frames implicitly.

## Render World

- Right-handed.
- `+X` right.
- `+Y` up.
- `+Z` toward viewer (camera convention).
- Units: normalized display units. Earth radius = 1.0 for rendering.

## Render View

- Camera looks toward `-Z`.
- `+X` right.
- `+Y` up.
- `-Z` forward.

## Projection

- NDC -> viewport is the only place Y is flipped.
- Aspect ratio uses **virtual framebuffer** (`cols*2`, `rows*4`), not cell counts.

## Astronomical / Orbital Frames

| Frame | Origin | Axis | Units | Usage |
|-------|--------|------|-------|-------|
| TEME | Earth center | Mean equator of epoch | km | SGP4 output |
| PEF | Earth center | True equator, Greenwich | km | After GMST/UT1 |
| ITRF | Earth center | ITRF2020 / WGS84 | km / m | SceneSnapshot if real tracking |
| ECEF | Earth center | ITRF-aligned | m | `geodetic_to_ecef` |
| ENU | Observer local tangent plane | East, North, Up | m | Observer geometry |
| GCRF / J2000 | Solar System barycenter / geocenter | ICRF-aligned | km | Horizons, future ephemeris |
| ECLIPTIC | Heliocentric | Ecliptic plane | km / AU | Solar System |

Rules:

- `NO POSITION WITHOUT FRAME`.
- `NO STATE WITHOUT EPOCH`.
- Render `World` is never an orbital frame. Use explicit converters `TEME->PEF->ITRF` and `ITRF->WGS84`.
- Never reuse a solar-system ephemeris directly as a render-world position without a frame transform.
- 4D hyperprojection uses a separate `w` channel and is explicitly tagged (`4D_TIME`, `4D_PHASE`).
- `ReferenceFrame` defines orientation and reference-frame identity; future `PhysicalState` origin semantics remain explicit and separate.
- WGS84 is a geodetic reference system, not an orbital `FrameKind`; use it only after an explicit terrestrial Cartesian transform.

## Scale

- Physical distance and body radius are distinct scales.
- `TRUE_SCALE`: physical km preserved; bodies may be invisible.
- `DISPLAY_SCALE`: body radius exaggerated; distance semi-preserved.
- HUD must always show both scales when applicable.

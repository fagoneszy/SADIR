# Render pipeline

`Renderer3D` consumes an immutable `SceneSnapshot`; it never reads orbit, frame, or
time services. The scene has already established its frame, origin, epoch, entities,
and SI-metre coordinates.

Each primitive follows this sequence:

1. `DisplayTransform` maps physical metres to renderer units, around an explicit
   physical origin.
2. `Camera::world_to_view` moves those units to view space.
3. Segments are clipped to the positive near/far depth interval before projection.
4. Perspective projection yields NDC coordinates. Segments are then clipped to the
   NDC square `[-1, 1] x [-1, 1]`; inverse depth is interpolated through this step.
5. NDC is mapped to virtual phosphor pixels. Rasterization performs one depth test
   per sample, and only the nearest positive finite depth injects phosphor intensity.
6. `Presenter` converts the phosphor buffer and HUD into terminal cells.

Labels are projected and retained as renderer metadata, ordered by descending
priority and then nearest depth. They do not alter scene state.

## Invariants

- The render core is deterministic for a given snapshot, camera, transform, and
  delta time.
- Invalid scenes, non-finite geometry, and invalid depth samples are rejected.
- The depth buffer is cleared every frame; phosphor persistence is independent of it.
- Clipping and rendering are visual operations only. They do not perform frame
  conversion, unit conversion, ephemeris evaluation, or orbital propagation.

## Interactive check

Build `nadir_render_demo` and run it in a real terminal. It shows a rotating wireframe
cube through the same scene renderer. Arrow keys adjust the camera; Escape exits.

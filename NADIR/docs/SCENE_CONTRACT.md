# Scene Contract

One scene has one epoch, one frame, and one origin.

Every `SceneObject` state and every primitive in a `SceneSnapshot` is expressed in
that declared space. `SceneBuilder` rejects a state with a different epoch, frame,
or origin; it never performs an implicit frame transformation. Objects retain
scientific metadata, while primitives carry only `entity_id` and render geometry.

Frame propagation and display-scale conversion are separate future layers.

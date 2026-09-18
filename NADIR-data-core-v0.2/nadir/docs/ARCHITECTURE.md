# Architecture

```text
Authoritative Sources
        |
        v
Source Registry
        |
        v
Acquisition + Rate Control
        |
        v
Raw Immutable Cache + SHA-256
        |
        v
Parsers / Normalizers
        |
        v
Time + Reference Frames
        |
        v
Scientific Data Model
        |
        +----------+----------+----------+
        |          |          |          |
      Earth      Orbit      Space      Network
        |          |          |          |
        +----------+----------+----------+
                   |
                   v
              Scene Graph
                   |
                   v
             Braille/ANSI
```

Raw source data should be retained separately from normalized data. Derived values should carry links back to raw source snapshots.

No renderer module should directly perform arbitrary network acquisition.

The current v0.2 build introduces the source registry and raw immutable cache. Parser and normalization layers will be expanded source by source.

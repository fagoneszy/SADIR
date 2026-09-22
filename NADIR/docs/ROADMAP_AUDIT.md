# Roadmap audit

Status is evidence-based: **done** means a current implementation and a named
test/gate; **foundation** means useful code exists but does not satisfy the
whole roadmap item; **open** means no sufficient implementation exists.

| Items | Status | Current evidence / remaining work |
|---|---|---|
| 0 workspace hygiene | done | `git diff --check`, clean commits; do not version build/cache. |
| 1–2 SGP4/Vallado | done | `nadir_sgp4_verification`, science label. |
| 3–4 OMM/provenance | foundation | JSON/KVN/XML parse, KVN export and cache provenance; full CCSDS profiles remain open. |
| 5–7 frames/EOP | foundation | TEME→PEF→ITRF plus Vallado Appendix-C oracle; full SOFA/IAU 2000/2006 suite remains open. |
| 8–13 WGS84/observer/passes/tracks/eclipse/Doppler | done | Dedicated WGS84, tracker, pass, ground-track, eclipse and sun tests. |
| 14–16 ISS/inspector/picking | foundation | Cached OMM path, inspector and deterministic picking exist; continuous live tracking UI is open. |
| 17–19 catalog/performance/SoA | foundation | aliases, batches and snapshot buffer exist; worker pool and large-catalog benchmark are open. |
| 20–21 LOD/quality | foundation | renderer LOD and inspector freshness labels; uncertainty visualization is open. |
| 22–23 covariance/conjunction | foundation | covariance transforms and deterministic screening; PSD/Pc and spatial broadphase are open. |
| 24 CCSDS | foundation | OMM, OPM KVN, single-segment OEM KVN, CDM conjunction-summary KVN and range/Doppler TDM KVN; OCM and broader profiles remain open. |
| 25–27 NDR/clock/replay | foundation | checksummed chronological NDR and deterministic clock; typed source provenance and Cartesian-state blocks exist, while additional schemas and UI replay are open. |
| 28–29 maps/stations | foundation | geodetic station catalog parser and TSV catalog/info console commands; Natural Earth data/rendering and a sourced public station catalog remain open. |
| 30–32 GNSS/ILRS | open | SP3/CLK/SSR, RINEX, CPF/CRD and validation fixtures absent. |
| 33 numerical propagation | foundation | central, J2, third-body and caller-parameterized exponential drag with RK4/adaptive RK4; validated atmosphere, higher-order integration and SRP remain open. |
| 34–37 EGM2008/atmosphere/weather/WMM | open | no validated model/data ingest. |
| 38–41 DE/SPICE/solar system/small bodies | foundation | Horizons provider and body data; local DE/SPICE and validated display path open. |
| 42 spacecraft geometry | foundation | OBJ plus tested ASCII and binary STL mesh loaders; GLB and provenance binding remain open. |
| 43–46 Gaia/4D/network/geospatial layers | open | no implementation sufficient for roadmap scope. |
| 47 fuzzing | foundation | deterministic mutation sweeps cover OMM, OPM, OEM, CDM and TDM; libFuzzer campaigns and remaining parsers are open. |
| 48 ingestion security | foundation | cache hashes, parser limits, HTTPS-only transport and 64 MiB response cap; content-type/schema policy and broader TLS controls remain open. |
| 49 benchmarks | foundation | deterministic SGP4 batch workload and JSON runner with machine metadata; renderer, memory and wider-catalog suites remain open. |
| 50 science gate | foundation | Vallado SGP4 + TEME/ITRF oracle; wider science suite open. |
| 51 network gate | open | no separate online-test gate. |
| 52–54 licenses/cache/release | foundation | data notice, cache metadata and manifest scripts; reproducible release artifacts/SBOM open. |
| 55 manual | foundation | README/docs exist; operational offline/update troubleshooting manual open. |
| 56 cleanup | foundation | ignore/gate tooling exists; final repository release audit open. |

Run the current deterministic suite with:

```powershell
cmake --build .\build --parallel 1
ctest --test-dir .\build --output-on-failure
cmake --build .\build --target science-gate --parallel 1
```

No row marked **foundation** or **open** may be represented as complete in
release notes.

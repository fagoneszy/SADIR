# Operational manual

Run these commands from the `NADIR` directory so relative configuration and
cache paths resolve to `config/` and `data/cache/`.

## Build and deterministic validation

In a Visual Studio x64 developer shell:

```powershell
cmake -S . -B build
cmake --build build --parallel 1
ctest --test-dir build --output-on-failure
cmake --build build --target science-gate --parallel 1
```

Run the non-deterministic HTTPS transport check separately (it accesses the
public `https://example.com` endpoint and is intentionally not part of CTest):

```powershell
cmake --build build --target network-gate --parallel 1
```

Create a non-overwriting Windows artifact with binary and SHA-256 sums:

```powershell
.\tools\package-release.ps1
```

## Local performance measurement

```powershell
.\build\nadir_orbit_benchmark.exe 1000
```

The benchmark reports SGP4 batch throughput for a deterministic synthetic OMM
population. It is a local measurement, not a pass/fail gate or an accuracy
fixture.

For a comparable JSON artifact with workload and machine metadata (without
overwriting an existing result):

```powershell
.\tools\run-benchmarks.ps1 -Objects 10000
```

Use `-Force` only when replacing the default artifact intentionally.

The science gate currently covers the Vallado SGP4 verification set, the
Vallado TEME-to-ITRF reference case, numerical central/J2/third-body/adaptive
propagation with drag and SRP, and encounter-plane collision probability. It
does not certify every future model or an operational conjunction decision.

## Data synchronization and offline mode

```powershell
.\build\nadir.exe sources orbit
.\build\nadir.exe sync preset:orbit-live
.\build\nadir.exe sync celestrak.satcat
.\build\nadir.exe cache celestrak.stations
.\build\nadir.exe orbit inspect stations ISS -12.093 -45.786 850 145800000
```

Acquisitions are stored below `data/cache/<source-id>/`. Each snapshot has a
`.meta` manifest with URL, retrieval time, byte count, HTTP status, licence
label and SHA-256. `cache <source-id>` displays the active snapshot; an orbit
command can run from that cached data without performing `orbit live` again.

To operate offline, do not invoke `sync`, `orbit live`, `eop live`, `body live`
or other `live` commands. Use cached source commands instead. If no snapshot
exists, the program reports the missing cache rather than silently fabricating
data.

## Cache integrity and recovery

Run this before relying on a cached source after a disk restore, interrupted
copy or manual transfer:

```powershell
.\build\nadir.exe cache <source-id>
```

`VERIFY PASS` means that the active payload exists, its byte count matches the
sidecar and its SHA-256 equals the recorded digest. `VERIFY FAIL` means the
snapshot is unsuitable for scientific use; preserve it for diagnosis, then
refresh the same source while online. Never edit `.meta` files to force a
successful verification.

The HTTPS transport rejects non-HTTPS URLs and limits one response to 64 MiB.
If a legitimate product exceeds that limit, add a bounded, format-specific
ingestion path and fixture before raising the limit globally.

## Data freshness

`orbit inspect` shows the catalog ingest time, cache age, quality and EOP
availability. `STALE` means the cache age exceeds twice the source polling
interval. `ESTIMATED` means no current IERS EOP was available; it is not an
accuracy claim.

## Troubleshooting

- `NO ORBIT CACHE`: run `orbit live <source|group>` when online, then retry.
- `OMM PARSE FAILED`: inspect `cache <source-id>` and retain the raw cached
  snapshot for diagnosis; do not overwrite it manually.
- `SGP4 FAILED`: the catalog record is invalid or decayed at the requested
  epoch; use another record or refresh the source.
- Missing compiler: open a Visual Studio x64 developer shell before CMake.
- Failing gate: run the displayed individual CTest name with
  `ctest --test-dir build -R <name> --output-on-failure`.
- `VERIFY FAIL`: do not use the active cache snapshot; refresh online or
  restore a known-good snapshot plus its matching `.meta` file.

# Threat Model v0.3

## Assets

- raw scientific snapshots
- provenance metadata
- future provider credentials
- normalized scientific records
- local network telemetry
- replay/recording files

## Trust boundaries

```text
Internet provider
      |
      v
HTTP acquisition
      |
      v
raw untrusted bytes
      |
      v
parser boundary
      |
      v
normalized internal model
      |
      v
renderer/analysis
```

All remote content is untrusted input even when the authority is trusted.

## Current controls

- HTTPS source URLs where providers support HTTPS
- immutable cache snapshots
- SHA-256 content digests
- separate raw and normalized layers
- per-source refresh intervals
- explicit authentication classes
- no automatic credential harvesting
- no dark-web/leak ingestion path
- bounded JSON parser structures through normal process memory limits
- source IDs converted to filesystem-safe cache paths

## Required before credentialed adapters

- OS credential-store integration
- secret redaction in logs
- zero credentials in source registry URLs
- provider-specific quota handling
- TLS certificate verification tests
- request timeout and size limits
- parser fuzzing
- signed source snapshots for internal deployments
- least-privilege service accounts

## Required before packet capture

Packet capture must be opt-in, interface-scoped and local to systems/networks the operator is authorized to observe. Raw packet retention should be separately configurable from aggregate flow telemetry.

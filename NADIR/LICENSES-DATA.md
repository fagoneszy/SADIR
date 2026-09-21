# Data-source notices

NADIR does not redistribute live acquisition data in source releases. Each
configured source carries its own authority, URL, coverage and license label in
`config/sources.tsv`; each acquired cache snapshot copies those fields into its
metadata manifest together with its SHA-256 digest and retrieval time.

Before redistributing a cached snapshot, consult the authoritative terms at its
recorded URL. A label such as `public-data` is descriptive metadata, not a
replacement for the source's licence or attribution requirements.

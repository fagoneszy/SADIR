# Spacecraft Architecture Model

NADIR separates orbital identity from spacecraft architecture.

An object record answers where an object is. An architecture profile answers what public evidence says the spacecraft class contains.

## Normalized architecture fields

```text
family
generation
manufacturer/operator
bus
structure
mass
dimensions
power generation
energy storage
propulsion
ADCS/navigation
flight compute
communications
antennas
optical links
payload/instruments
thermal control
radiators
solar arrays
3D/CAD model
technical-document references
```

## Seed profiles in v0.3

```text
Starlink V2
Starlink V3
James Webb Space Telescope
Hubble Space Telescope
Voyager 1/2 architecture
International Space Station
```

These profiles contain only values directly tied to public operator/agency material. They are not reverse-engineered internal schematics.

## Scaling to the catalog

The intended hierarchy is:

```text
orbital object instance
        |
        v
spacecraft model/class
        |
        v
bus/platform family
        |
        v
subsystems/components
        |
        v
evidence records
```

Future architecture enrichment will query NASA NTRS/PDS/3D resources, NAIF mission kernels, ESA DISCOS, agency mission documentation, operator documentation and public regulatory filings. Where only size/mass/mission metadata are public, NADIR stores only those fields.

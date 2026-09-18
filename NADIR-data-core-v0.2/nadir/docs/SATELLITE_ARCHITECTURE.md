# Satellite Architecture Model

NADIR separates a spacecraft instance from its spacecraft class.

An orbital catalog object answers where an object is. An architecture profile answers what publicly documented hardware the class contains.

## Instance

```text
NORAD/USSF catalog id
international designator
name
launch
operator
orbit source
spacecraft class id
```

## Class

```text
manufacturer
generation
bus
structure
mass
dimensions
power
propulsion
ADCS
navigation
communications
payload
thermal
compute
3D model
technical documentation
```

## Starlink public profiles

The starter profile contains only fields stated by official Starlink public material.

V2/current public material includes a compact flat-panel form factor, three optical inter-satellite links up to 200 Gbps each, five Ku-band phased-array antennas, three dual-band Ka/E antennas, argon ion propulsion, dual solar arrays, a high-capacity battery and a custom star tracker.

The public V3 update states six 400 Gbps optical links, four quad-band Ka/E/V/W RF backhaul antennas, up to 1.2 Tbps RF backhaul capacity, approximately twice the V2 solar-array power and a solar blanket assembled from four 19 m segments.

NADIR does not infer an internal circuit diagram, exact processor, undisclosed dimensions or proprietary payload construction when no public source supports it.

## Future architecture acquisition

Priority sources:

```text
operator/manufacturer documentation
NASA mission pages
NASA 3D Resources
NASA NTRS
NASA PDS
NAIF SPICE frame/instrument kernels
ESA/agency mission documentation
FCC and other public regulatory filings
technical papers
public CAD/mesh assets
```

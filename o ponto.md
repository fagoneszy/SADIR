# NADIR - Navigational & Astronomical Data Instrumentation Renderer

## Visão Geral

**NADIR** é um console científico/espacial em tempo real, local-first, controlado inteiramente pelo terminal, capaz de visualizar Terra, órbitas, satélites reais, modelos 3D, telemetria, sistemas, redes e BGP global.

### Regra Absoluta
> **Nada aparece na tela só para parecer tecnológico. Cada linha, número, ponto, arco e vetor deve representar alguma coisa real.**

---

## Identidade Visual

A identidade fica entre **Soyuz + DOS científico + terminal UNIX + instrumento de laboratório dos anos 80/90**.

- Não "Cyberpunk 2077"
- Três temas definidos:
  - **PHOSPHOR-G**: #00ff77, #0a2819, #000000
  - **AMBER-OPS**: #ffb44a, #3a2100, #050300
  - **MONO-MISSION**: #d8ddd5, #737873, #000000
- Vermelho reservado exclusivamente para: ERROR, ALERT, LOSS, COLLISION, WITHDRAWAL, INVALID

---

## Linguagem e Stack Tecnológica

| Camada | Tecnologia |
|--------|-----------|
| Aplicação principal | **C++23** |
| APIs do SO / interoperabilidade | **C17** |
| Kernels específicos | **x86-64 Assembly / NASM** |
| SIMD | AVX2 inicialmente |
| Build | CMake + Ninja |
| Terminal | ANSI/VT diretamente |
| Windows | Win32 Console API |
| Linux | termios + ioctl |
| Rede | sockets nativos + libpcap/Npcap |
| HTTP | libcurl ou implementação mínima própria |
| WebSocket | implementação C++ enxuta |
| Armazenamento | arquivos binários + mmap |
| Render | **software renderer próprio** |

- Sem Electron, React, WebView, engine, Unity, Qt no núcleo
- Programa inicia em **menos de um segundo**
- Funciona offline com dados cacheados
- Consome pouquíssima RAM
- Assembly reservado para rasterização, transformações vetoriais e packing SIMD após benchmarking

---

## Arquitetura de Dados

```text
DATA
  ↓
INGEST
  ↓
NORMALIZATION
  ↓
PHYSICS / MATH
  ↓
SCENE GRAPH
  ↓
SOFTWARE RENDERER
  ↓
TERMINAL FRAMEBUFFER
  ↓
ANSI / BRAILLE / ASCII
```

---

## Sistemas de Coordenadas

NADIR sabe a diferença entre:

- **Geodetic coordinates**: Latitude / Longitude / Altitude
- **ECEF**: Earth-Centered Earth-Fixed
- **ECI / GCRF**: Earth-Centered Inertial
- **TEME**: True Equator Mean Equinox
- **ENU**: East North Up
- **Azimuth / Elevation**
- **RA / DEC**
- **UTC / TAI / TT / UT1**

---

## Terra - WGS 84 Oficial

Parâmetros da NGA:

```text
semi-major axis: a = 6378137.0 m
inverse flattening: 1/f = 298.257223563
GM: 3.986004418e14 m³/s²
mean angular velocity: 7.292115e-5 rad/s
```

### Conversão Geodésica → ECEF (C++23)

```cpp
#include <cmath>

struct Vec3 {
    double x;
    double y;
    double z;
};

constexpr double WGS84_A = 6378137.0;
constexpr double WGS84_INV_F = 298.257223563;
constexpr double WGS84_F = 1.0 / WGS84_INV_F;
constexpr double WGS84_E2 = WGS84_F * (2.0 - WGS84_F);
constexpr double DEG = 3.14159265358979323846 / 180.0;

Vec3 geodetic_to_ecef(
    double latitude_deg,
    double longitude_deg,
    double altitude_m)
{
    const double lat = latitude_deg * DEG;
    const double lon = longitude_deg * DEG;

    const double sin_lat = std::sin(lat);
    const double cos_lat = std::cos(lat);
    const double sin_lon = std::sin(lon);
    const double cos_lon = std::cos(lon);

    const double N = WGS84_A / std::sqrt(
        1.0 - WGS84_E2 * sin_lat * sin_lat
    );

    return {
        (N + altitude_m) * cos_lat * cos_lon,
        (N + altitude_m) * cos_lat * sin_lon,
        (N * (1.0 - WGS84_E2) + altitude_m) * sin_lat
    };
}
```

---

## Terminal como Framebuffer

- **Unicode Braille** como pipeline de renderização
- Cada caractere Braille = matriz 2×4 = 8 pixels virtuais
- Terminal 160×50 ≈ 320×200 pontos gráficos
- Pipeline: 3D vertex → world transform → view transform → perspective projection → near-plane clipping → screen coordinates → Bresenham/rasterizer → virtual pixel buffer → Braille encoder → ANSI terminal

---

## Satélites Reais

### TLE / SGP4

- CelesTrak disponibiliza dados GP em TLE, 2LE, XML, KVN, JSON, CSV
- SGP4 para propagação orbital
- Space-Track com controle de frequência (< 30 chamadas/minuto, 300/hora)

```text
nadir sat search ISS
OBJECT: ISS (ZARYA)
NORAD: 25544
TYPE: PAYLOAD
PROPAGATOR: SGP4
SOURCE: CELESTRAK

nadir sat track 25544
ALTITUDE: 418.23 km
VELOCITY: 7.66 km/s
AZIMUTH: 134.84 deg
ELEVATION: 38.91 deg
RANGE: 1041.13 km
RANGE RATE: -3.24 km/s
LAT: -4.22 deg
LON: -38.73 deg
AOS: 23:44:12 UTC
MAX EL: 71.2°
LOS: 23:51:53 UTC
```

### Ground Track

```text
nadir sat groundtrack 25544
```

Com continentes reais por cima (Natural Earth).

---

## BGP Global em Tempo Real

### RIPE RIS Live

```text
nadir bgp live
```

Mostra anúncios BGP ao vivo:

```text
AS1299
   │
   ├──── AS3356
   │       │
   │       └── AS13335
   │
   └──── AS2914
           │
           └── AS15169
```

### BGP no Globo

```text
nadir bgp globe
```

Linhas correspondentes a eventos entre ASNs/geolocalizações conhecidas, com persistência de fósforo de CRT.

---

## Modelos 3D

```text
nadir model import spacecraft.obj
nadir model inspect spacecraft.obj
nadir model wire spacecraft.obj
nadir model explode spacecraft.obj
```

Render wireframe interativo de modelos oficiais da NASA.

---

## Sistema Solar - JPL Horizons

```text
nadir body mars
nadir body moon
nadir body jupiter
```

Posição, velocidade, range, light time, RA, DEC, phase, apparent magnitude, observer geometry.

---

## Network Visualization

```text
nadir net interfaces
nadir net routes
nadir net arp
nadir net connections
nadir net capture
nadir net topology
```

5-tuple flows:

```text
192.168.1.21
   ├──── TCP/443 ───────────────► 104.x.x.x (3.7 MB)
   │
   ├──── UDP/53 ────────────────► 1.x.x.x (DNS)
   │
   └──── TCP/22 ────────────────► 10.0.0.7 (SSH)
```

---

## Data Provenance

Cada objeto tem:

```text
SOURCE
FETCHED_AT
EPOCH
FORMAT
CHECKSUM
AGE
REFERENCE_FRAME
PROPAGATOR
PRECISION
```

Exemplo:

```text
ISS (25544)
SOURCE: CELESTRAK
FORMAT: OMM
FETCHED: 2026-09-17T23:20:14Z
EPOCH: 2026-09-17T18:42:17Z
THEORY: SGP4
FRAME: TEME
AGE: 04:37:57
SHA256: D91F...
```

---

## Formato de Arquivo Próprio (.ndr)

### Header

```cpp
struct NadirHeader {
    char     magic[8];       // NADIR\0\0\1
    uint16_t version;
    uint16_t type;
    uint64_t timestamp_ns;
    uint64_t record_count;
    uint32_t flags;
    uint32_t crc32;
};
```

### Blocos

```
META, SOURCE, OBJECT, ORBIT, MESH, TELEMETRY, NETWORK, EVENT
```

```text
nadir record mission.ndr
nadir replay mission.ndr
nadir replay mission.ndr --speed 25
```

---

## Comandos TUI

```
NADIR 0.1.0
Scientific systems console

nadir> earth
nadir> earth grid 10
nadir> observer set -12.09 -45.78 740
nadir> sat search ISS
nadir> sat track 25544
nadir> sat groundtrack 25544
nadir> sat passes 25544
nadir> model load ISS.obj
nadir> model wire
nadir> body moon
nadir> net capture Ethernet
nadir> net flows
nadir> bgp live
nadir> bgp globe
nadir> source status
nadir> record session.ndr
```

---

## Roadmap (Fases N0-N18)

| Fase | Resultado |
|------|-----------|
| **N0** | CMake, terminal raw, framebuffer, input, ANSI |
| **N1** | Matemática vetorial, Matrix4, quaternion, câmera |
| **N2** | Renderer 3D wireframe + Braille |
| **N3** | Esfera WGS84, coordenatas, meridianos, paralelos |
| **N4** | Continentes vetoriais e Earth coordinate systems |
| **N5** | Parser TLE/OMM + SGP4 |
| **N6** | ISS e qualquer objeto NORAD em tempo real |
| **N7** | Ground track, estação, az/el, AOS/LOS, Doppler |
| **N8** | IERS EOP e sistemas UTC/TAI/TT/UT1 |
| **N9** | JPL Horizons / Lua / planetas |
| **N10** | OBJ/STL + satélites NASA 3D |
| **N11** | Interface de telemetria completa |
| **N12** | Packet capture + flows |
| **N13** | BGP global RIPE RIS Live |
| **N14** | Replay/record `.ndr` |
| **N15** | SIMD/AVX2 + Assembly |
| **N16** | Testes científicos e comparação com GMAT/Horizons |
| **N17** | Sistema completo de plugins/source adapters |
| **N18** | Documentação técnica estilo flight software |

Começa-se **exatamente por N0–N3**, e não por API de satélite.

---

## Fontes Técnicas Principais

1. **CelesTrak**: GP/OMM formats e SGP4 documentation
2. **NASA/JPL Horizons**: Ephemerides API
3. **IERS**: Earth Orientation Parameters (EOP)
4. **NGA**: Definição WGS 84
5. **RIPE**: BGP real em tempo real
6. **NASA 3D Resources**: Modelos spacecraft
7. **NASA GMAT R2026**: Validação externa open-source

> **"Nadir seria um computador científico fictício na aparência, mas completamente real por dentro."**

---

## Referências Cruzadas

[1] NGA Geomatics - WGS 84: https://earth-info.nga.mil/?action=wgs84&dir=wgs84
[2] CelesTrak: https://www.celestrak.org/NORAD/documentation/gp-data-formats.php
[3] Space-Track: https://www.space-track.org/documentation
[4] Natural Earth: https://www.naturalearthdata.com/downloads/10m-physical-vectors/
[5] IERS Datacenter: https://datacenter.iers.org/eop.php
[6] JPL Horizons: https://ssd.jpl.nasa.gov/horizons/manual.html
[7] NASA 3D Resources: https://www.nasa.gov/stem-content/nasa-3d-resources/
[8] RIPE RIS Live: https://ris-live.ripe.net/
[9] NASA GMAT: https://software.nasa.gov/software/GSC-19640-1
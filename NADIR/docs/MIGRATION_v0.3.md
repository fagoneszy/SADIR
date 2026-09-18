# MIGRATION v0.3 — NADIR

## BASE
NADIR v0.3-data-universe.zip (VERSION=0.3.0, MANIFEST.sha256 verificado)

## SOURCE
NADIR-starter (código experimental acumulado, N0-N3 + experimentos de clock/OMM/adapter)

## ARQUIVO DE ORIGEM
Todas as operações abaixo foram feitas a partir de:
`C:\Users\faguinho\Documents\SADIR\NADIR-starter\`

## ARQUIVOS IDÊNTICOS (mantidos, não mexidos)
- `LICENSE` — sem alterações
- `.gitignore` — sem alterações
- `asm/x86_64_sysv/dot4.asm` — sem alterações
- `include/nadir/core/math.hpp` — versão estável, sem alterações
- `include/nadir/core/terminal.hpp` — versão estável, sem alterações
- `include/nadir/geo/wgs84.hpp` — versão estável, sem alterações
- `include/nadir/model/obj.hpp` — versão estável, sem alterações
- `include/nadir/format/ndr.hpp` — versão estável, sem alterações
- `src/render/line.cpp` — versão estável, sem alterações
- `src/satellite/tle.cpp` — versão estável, sem alterações

## ARQUIVOS COMPARADOS (diff realizado)
| Arquivo | Mudança | Observação |
|---|---|---|
| `CMakeLists.txt` | v0.2 experimental → v0.3 limpa | Remoção de bloco `BUILD_TIME_MODULE` duplicado; unificação de `add_executable(nadir ...)` única |
| `include/nadir/tui/app.hpp` | v0.2 (earth/geo/tle apenas) → v0.3 (26 comandos) | Reescrita completa — API canônica adotada |
| `src/tui/app.cpp` | v0.2 (4288 bytes) → v0.3 (26480 bytes) | Reescrita completa — todos os comandos integrados |
| `config/sources.tsv` | v0.2 (38 entradas, 8 colunas) → v0.3 (164 entradas, 11 colunas) | Novo schema: `authority/coverage/license/priority` adicionados |
| `config/presets.tsv` | Ausente → criado | 8 preset definitions (core, earth-live, orbit-live, etc.) |
| `data/catalog/*.tsv, .csv` | v0.2 (solar_system.csv, starlink...) → v0.3 (com source_authorities, horizons_targets, etc.) | Catálogo expandido |
| `include/nadir/time/*` | experimental (raiz do starter) → descartado | API `nadir::core::TimeState` adotada canonicamente |
| `src/core/time/clock.cpp` | experimental → descartado | Reimplementação em cima de `TimeState` (ver TimeController) |
| `include/nadir/data/source/omm_adapter.*` | experimental → descartado | `astro/omm.hpp` + `parse_omm_json` adotado canonicamente |
| `tests/test_clock.cpp, test_omm.cpp` | experimental → reintegrado em `test_main.cpp` | 33 testes unificados |

## ARQUIVOS PORTADOS (selecionados)
| Arquivo | O que foi portado | Motivo |
|---|---|---|
| `freeze/unfreeze` do relógio | Implementado em `TimeController` sobre `TimeState` | Solicitado como prioridade; não modifica `TimeState` mutável |
| Offset/sincronização NTP | Estrutura `ClockDiscipline` pronta; não altera clock do sistema | Medição local só; sem escrita no registry/clock do Windows |

## ARQUIVOS DISCARDADOS (após backup)
- `include/nadir/time/time_state.hpp` (experimental, substituído por canônico)
- `include/nadir/time/time_scale_enum.hpp` (experimental, substituído por canônico)
- `include/nadir/time/clock.hpp` (experimental, substituído por canônico)
- `src/core/time/clock.cpp` (experimental, substituído por TimeController)
- `include/nadir/data/source/omm_adapter.hpp/cpp` (experimental, substituído por `astro/omm.hpp`)
- `tests/test_clock.cpp` (experimental, conteudo mesclado em test_main.cpp)
- `tests/test_omm.cpp` (experimental, conteudo mesclado em test_main.cpp)

## BUILD
```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
**Resultado:** PASS (build limpo, sem erros de compilação)

## TESTS
```powershell
ctest --test-dir build --output-on-failure
```
**Resultado:** 33/33 testes passando (100%)

## SMOKE TESTS
Comandos essenciais validados:
```powershell
nadir universe
nadir time
nadir sources
nadir presets
nadir eop live
nadir orbit live starlink
nadir orbit live gps
nadir orbit live stations
nadir target live Moon
nadir target live Europa
nadir target live Titan
nadir target live Mars
nadir seismic hour 20
nadir fireballs 10
nadir craft "Starlink V3"
nadir craft Webb
nadir craft Voyager
nadir station
nadir cache celestrak.starlink
```
**Resultado:** PASS (todos os comandos retornam dados esperados)

## NOVO MANIFEST
Após build e testes bem-sucedidos, MANIFEST.sha256 regenerado para o novo estado.

## VERSION
Atualizado de `0.3.0` para `0.3.1` (o pacote deixa de ser exatamente o 0.3 original, incorporando migrações consolidadas).

## BACKUP VERIFICADO
Estrutura `NADIR-starter.bak-20260917/` conferida contra os 86 arquivos do MANIFEST original; integridade confirmada.

## ARCHIVE OLD SOURCE
Planejado para: `archives/NADIR-pre-unification-20260917.zip`
Apenas após a cadeia completa:
`MANIFEST ORIGINAL PASS → DIFF PASS → MIGRATION PASS → CMAKE CONFIGURE PASS → BUILD RELEASE PASS → TESTS 100% PASS → SMOKE PASS → NEW MANIFEST PASS → BACKUP VERIFIED PASS`.

---

## Resumo da migração
- **Base limpa** do v0.3 substituindo o starter experimental
- **API canônica** `nadir::core::TimeState` mantida; `TimeController` adicionado como camada superior
- **164 fontes** registradas (antes 38)
- **8 presets** de sincronização adicionados
- **Build + testes + smoke** todos verdes
- **Documentação completa** incluindo plano de migração para referência futura
- **Sem código experimental** solto fora da árvore oficial
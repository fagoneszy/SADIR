#include <nadir/astro/cpf.hpp>
#include <cmath>
#include <string>
int main(){const auto parsed=nadir::astro::parse_cpf("H1 CPF 2 AIU 2026 1 1 0 1 lageos1\nH2 7603901 0 8820 2026 1 1 0 0 0 2026 1 2 0 0 0 900 1 1 0 0 0\nH9\n10 0 61000 86387.0 0 -13785362.868 -12150743.695 19043830.747\n99\n");if(!parsed.ok||!parsed.positions||parsed.positions->size()!=1)return 1;const auto& point=parsed.positions->front();if(point.direction||point.modified_julian_date!=61000||std::abs(point.position_m.z-19043830.747)>1e-6)return 2;if(nadir::astro::parse_cpf("H1 CPF 2\nH2 x\nH9\n10 0 1 0 0 1 2\n99\n").ok)return 3;if(nadir::astro::parse_cpf(std::string(16U*1024U*1024U+1U,'x')).ok)return 4;return 0;}

#include <nadir/astro/clk.hpp>
#include <cmath>
#include <string>
int main() {
 const auto parsed=nadir::astro::parse_rinex_clk("     3.04           C                   RINEX VERSION / TYPE\n                                                            END OF HEADER\nAS G01 2026 01 01 00 00  0.000000000  2    1.234567890123E-04  1.0E-12\nAS G02 2026 01 01 00 00 30.000000000  1   -2.500000000000E-05\n");
 if (!parsed.ok || !parsed.record || parsed.record->samples.size()!=2 || std::abs(parsed.record->samples[0].clock_bias_s-1.234567890123e-4)>1e-16 || !parsed.record->samples[0].clock_bias_sigma_s) return 1;
 if (parsed.record->samples[0].epoch!="2026-01-01T00:00:00.000000000" || parsed.record->samples[1].clock_bias_sigma_s) return 2;
 return nadir::astro::parse_rinex_clk("AS G01 2026 1 1 0 0 0 1 0").ok || nadir::astro::parse_rinex_clk(std::string(16U*1024U*1024U+1U,'x')).ok ? 3 : 0;
}

#include <nadir/astro/rinex_nav.hpp>
#include <cmath>
#include <string>
int main(){const auto parsed=nadir::astro::parse_rinex_navigation("     3.04           N                   RINEX VERSION / TYPE\n                                                            END OF HEADER\nG01 2026 01 01 00 00 00  1.234567890123D-04 1.0D-12 -2.0D-20\n");if(!parsed.ok||!parsed.records||parsed.records->size()!=1||(*parsed.records)[0].satellite_id!="G01"||std::abs((*parsed.records)[0].clock_bias_s-1.234567890123e-4)>1e-16)return 1;if(nadir::astro::parse_rinex_navigation("G01 2026 1 1 0 0 0 1 2 3").ok)return 2;return nadir::astro::parse_rinex_navigation(std::string(16U*1024U*1024U+1U,'x')).ok?3:0;}

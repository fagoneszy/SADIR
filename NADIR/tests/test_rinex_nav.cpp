#include <nadir/astro/rinex_nav.hpp>
#include <cmath>
#include <cstdint>
#include <string>
int main(){
 const auto parsed=nadir::astro::parse_rinex_navigation("     3.04           N                   RINEX VERSION / TYPE\n                                                            END OF HEADER\nG01 2026 01 01 00 00 00  1.234567890123D-04 1.0D-12 -2.0D-20\n");
 if(!parsed.ok||!parsed.records||parsed.records->size()!=1||(*parsed.records)[0].satellite_id!="G01"||(*parsed.records)[0].gps||std::abs((*parsed.records)[0].clock_bias_s-1.234567890123e-4)>1e-16)return 1;
 const auto broadcast=nadir::astro::parse_rinex_navigation("     3.04           N                   RINEX VERSION / TYPE\n                                                            END OF HEADER\nG01 2026 01 01 00 00 00  1D-4 0 0\n 0 2 0 0\n 0 0 0 5153.7954775\n 0 0 0 0\n 0 0 0 0\n 0 0 0 0\n 0 0 0 0\n 0 0 0 0\n");
 if(!broadcast.ok||!broadcast.records||!(*broadcast.records)[0].gps)return 2;const auto state=nadir::astro::propagate_gps_broadcast((*broadcast.records)[0],0.0);const double a=5153.7954775*5153.7954775;if(!state||std::abs(state->position_m.x-a)>1e-3||std::abs(state->position_m.y)>1e-6||std::abs(state->position_m.z)>1e-6||std::abs(state->clock_correction_s-1e-4)>1e-15)return 3;
 if(nadir::astro::parse_rinex_navigation("G01 2026 1 1 0 0 0 1 2 3").ok)return 4;if(nadir::astro::parse_rinex_navigation(std::string(16U*1024U*1024U+1U,'x')).ok)return 5;std::uint32_t random=0x85ebca6bU;for(int sample=0;sample<512;++sample){const auto length=random%512U;random=random*1664525U+1013904223U;std::string fuzz;fuzz.reserve(length);for(std::uint32_t i=0;i<length;++i){random=random*1664525U+1013904223U;fuzz.push_back(static_cast<char>(random>>24U));}(void)nadir::astro::parse_rinex_navigation(fuzz);}return 0;
}

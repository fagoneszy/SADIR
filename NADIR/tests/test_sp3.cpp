#include <nadir/astro/sp3.hpp>
#include <cmath>
#include <string>
int main() {
    const auto parsed=nadir::astro::parse_sp3("#cP2026 01 01 00 00 00.00000000\n%c cc GPS ccc\n*  2026 01 01 00 00 00.00000000\nPG01  12345.678901  23456.789012  34567.890123      12.500000\n*  2026 01 01 00 15 00.00000000\nPG01  12346.000000  23457.000000  34568.000000  999999.999999\nEOF\n");
    if (!parsed.ok || !parsed.record || parsed.record->version!='c' || parsed.record->time_system!="GPS" || parsed.record->samples.size()!=2) return 1;
    const auto& first=parsed.record->samples.front();
    if (first.satellite_id!="G01" || std::abs(first.position_m.x-12'345'678.901)>1e-6 || !first.clock_offset_s || std::abs(*first.clock_offset_s-12.5e-6)>1e-15) return 2;
    if (parsed.record->samples.back().clock_offset_s) return 3;
    return nadir::astro::parse_sp3("PG01 1 2 3 4").ok || nadir::astro::parse_sp3(std::string(16U*1024U*1024U+1U,'x')).ok ? 4 : 0;
}

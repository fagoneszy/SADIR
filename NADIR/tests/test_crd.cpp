#include <nadir/astro/crd.hpp>
int main(){auto r=nadir::astro::parse_crd_normal_points("H1 CRD 2 2026 1 1 0\nH2 station 0 0 0 0\nH3 target 0 0 0\nH4 1 2026 1 1 0 0 0 2026 1 1 1 0 0 0 0 0 0 0\n11 123.0 0.012345 SYS 2 30.0 42 1 0 0 0 50 3 12\nH8\nH9\n");return !r.ok||!r.normal_points||r.normal_points->front().raw_ranges!=42?1:0;}

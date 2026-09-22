#include <nadir/orbit/batch.hpp>

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::size_t count = 1000;
    if (argc > 1) {
        try { count = std::stoull(argv[1]); } catch (...) { return 2; }
    }
    if (count == 0 || count > 100000) return 2;
    nadir::astro::OmmRecord seed{};
    seed.object_name="BENCH"; seed.norad_cat_id=25544; seed.epoch="2024-01-01T00:00:00.000";
    seed.mean_motion_rev_day=15.5; seed.eccentricity=0.0005; seed.inclination_deg=51.6;
    seed.raan_deg=20.0; seed.arg_pericenter_deg=40.0; seed.mean_anomaly_deg=10.0; seed.bstar=0.0001;
    std::vector<nadir::astro::OmmRecord> records(count, seed);
    for (std::size_t i=0; i<count; ++i) records[i].norad_cat_id += i;
    const auto start=std::chrono::steady_clock::now();
    const auto batch=nadir::orbit::propagate_sgp4_batch(records, 30.0, 0);
    const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    if (batch.size()!=count || elapsed<=0.0) return 1;
    std::cout<<"OBJECTS "<<count<<"\nSECONDS "<<std::fixed<<std::setprecision(6)<<elapsed
             <<"\nPROPAGATIONS_PER_SECOND "<<std::setprecision(2)<<count/elapsed<<"\n";
    return 0;
}

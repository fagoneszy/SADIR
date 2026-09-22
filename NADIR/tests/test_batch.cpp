#include <nadir/orbit/batch.hpp>

int main() {
    nadir::astro::OmmRecord iss{}; iss.norad_cat_id=25544; iss.epoch="2008-09-20T12:25:39.004000";
    iss.mean_motion_rev_day=15.72125391; iss.eccentricity=0.0006703; iss.inclination_deg=51.6416; iss.raan_deg=247.4627;
    iss.arg_pericenter_deg=130.5360; iss.mean_anomaly_deg=325.0288; iss.bstar=-0.000011606;
    const std::vector<nadir::astro::OmmRecord> records{iss, {}, iss, iss, iss};
    const auto batch=nadir::orbit::propagate_sgp4_batch(records,0.0,1);
    if (batch.size()!=5 || batch.norad_ids[0]!=25544 || batch.valid[0]!=1 || batch.valid[1]!=0) return 1;
    const auto parallel=nadir::orbit::propagate_sgp4_batch(records,0.0,3);
    if (parallel.norad_ids != batch.norad_ids || parallel.valid != batch.valid || parallel.position_x_km != batch.position_x_km ||
        parallel.position_y_km != batch.position_y_km || parallel.position_z_km != batch.position_z_km ||
        parallel.velocity_x_km_s != batch.velocity_x_km_s || parallel.velocity_y_km_s != batch.velocity_y_km_s ||
        parallel.velocity_z_km_s != batch.velocity_z_km_s) return 2;
    nadir::orbit::Sgp4BatchWorkerPool pool{3};
    const auto pooled = pool.propagate(records, 0.0);
    const auto pooled_again = pool.propagate(records, 15.0);
    const auto expected_again = nadir::orbit::propagate_sgp4_batch(records, 15.0, 1);
    if (pool.worker_count() != 3 || pooled.position_x_km != batch.position_x_km ||
        pooled_again.position_x_km != expected_again.position_x_km || pooled_again.valid != expected_again.valid) return 3;
    return batch.position_x_km.size()==batch.size() && batch.velocity_z_km_s.size()==batch.size() &&
        nadir::orbit::propagate_sgp4_batch({}, 0.0, 8).size()==0 && pool.propagate({}, 0.0).size()==0 ? 0 : 4;
}

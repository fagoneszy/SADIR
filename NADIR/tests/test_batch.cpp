#include <nadir/orbit/batch.hpp>

int main() {
    nadir::astro::OmmRecord iss{}; iss.norad_cat_id=25544; iss.epoch="2008-09-20T12:25:39.004000";
    iss.mean_motion_rev_day=15.72125391; iss.eccentricity=0.0006703; iss.inclination_deg=51.6416; iss.raan_deg=247.4627;
    iss.arg_pericenter_deg=130.5360; iss.mean_anomaly_deg=325.0288; iss.bstar=-0.000011606;
    const auto batch=nadir::orbit::propagate_sgp4_batch({iss,{}},0.0);
    if (batch.size()!=2 || batch.norad_ids[0]!=25544 || batch.valid[0]!=1 || batch.valid[1]!=0) return 1;
    return batch.position_x_km.size()==batch.size() && batch.velocity_z_km_s.size()==batch.size() ? 0 : 2;
}

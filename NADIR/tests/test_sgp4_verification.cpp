#include <nadir/astro/omm.hpp>
#include <nadir/orbit/sgp4.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::string tle_json = R"json([{"OBJECT_NAME":"ISS (ZARYA)","OBJECT_ID":"1998-067A","EPOCH":"2008-09-20T12:25:39.004000","NORAD_CAT_ID":"25544","MEAN_MOTION":"15.72125391","ECCENTRICITY":"0.0006703","INCLINATION":"51.6416","RA_OF_ASC_NODE":"247.4627","ARG_OF_PERICENTER":"130.5360","MEAN_ANOMALY":"325.0288","BSTAR":"-0.000011606","MEAN_ELEMENT_THEORY":"SGP4"}])json";
    auto tle_op = nadir::astro::parse_omm_json(tle_json);
    if (!tle_op.ok || tle_op.records.empty()) {
        std::cout << "parse failed\n";
        return 2;
    }
    const auto& rec = tle_op.records[0];

    auto pos_err = [](const nadir::orbit::Vec3d& a, const nadir::orbit::Vec3d& b) {
        auto d = a - b;
        return d.norm();
    };
    auto vel_err = [](const nadir::orbit::Vec3d& a, const nadir::orbit::Vec3d& b) {
        auto d = a - b;
        return d.norm();
    };

    struct Expected { double m; nadir::orbit::Vec3d p; nadir::orbit::Vec3d v; };
    std::vector<Expected> expected = {
        {0,   {4083.900545, -993.631911, 5243.600440}, {2.512840, 7.259888, -0.583779}},
        {60,  {-4133.793345, -4683.155177, -2526.661599}, {2.443902, -5.007590, 5.298599}},
        {120, {537.778823, 6247.338128, -2452.406166}, {-5.286183, -1.634624, -5.348149}},
    };

    double max_pe = 0.0, sum_pe2 = 0.0;
    double max_ve = 0.0, sum_ve2 = 0.0;
    int samples = 0;
    bool any_fail = false;

    for (const auto& exp : expected) {
        auto result = nadir::orbit::propagate_sgp4(rec, exp.m);
        if (!result) {
            std::cout << "propagate failed at m=" << exp.m << " err=" << nadir::orbit::to_string(result.error) << "\n";
            return 3;
        }
        double pe = pos_err(result.state.position_km, exp.p);
        double ve = vel_err(result.state.velocity_km_s, exp.v);
        max_pe = std::max(max_pe, pe);
        max_ve = std::max(max_ve, ve);
        sum_pe2 += pe * pe;
        sum_ve2 += ve * ve;
        ++samples;
        if (pe > 0.1 || ve > 0.001) any_fail = true;
        std::cout << "m=" << exp.m << " pos_err=" << pe << " km vel_err=" << ve << " km/s"
                  << " pos=(" << result.state.position_km.x << "," << result.state.position_km.y << "," << result.state.position_km.z << ")"
                  << " vel=(" << result.state.velocity_km_s.x << "," << result.state.velocity_km_s.y << "," << result.state.velocity_km_s.z << ")\n";
    }

    double rms_pe = std::sqrt(sum_pe2 / samples);
    double rms_ve = std::sqrt(sum_ve2 / samples);

    std::cout << "SGP4 VALLADO VERIFICATION samples=" << samples
              << " max_pos=" << max_pe << " rms_pos=" << rms_pe
              << " max_vel=" << max_ve << " rms_vel=" << rms_ve << "\n";

    if (any_fail) {
        std::cout << "SGP4 Vallado verification FAIL\n";
        return 10;
    }
    std::cout << "SGP4 verification PASS\n";
    return 0;
}

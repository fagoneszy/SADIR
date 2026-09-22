#pragma once
#include <nadir/math/vec3.hpp>
#include <optional>
#include <string>
#include <vector>
namespace nadir::astro {
struct GpsBroadcastEphemeris {
    double toe_s{}, sqrt_a_m_half{}, eccentricity{}, inclination_rad{}, inclination_rate_rad_s{};
    double right_ascension_rad{}, right_ascension_rate_rad_s{}, argument_of_perigee_rad{};
    double mean_anomaly_rad{}, mean_motion_delta_rad_s{};
    double cuc_rad{}, cus_rad{}, cic_rad{}, cis_rad{}, crc_m{}, crs_m{}, clock_group_delay_s{};
};
struct BroadcastState { math::Vec3d position_m; double clock_correction_s{}; };
struct RinexNavRecord { std::string satellite_id; std::string epoch; double clock_bias_s{}; double clock_drift_s_s{}; double clock_drift_rate_s_s2{}; std::optional<GpsBroadcastEphemeris> gps; };
struct RinexNavParseResult { bool ok{}; std::optional<std::vector<RinexNavRecord>> records; std::string error; };
// Parses bounded RINEX 3 navigation records, including GPS broadcast blocks.
RinexNavParseResult parse_rinex_navigation(const std::string& text);
// Propagates a GPS broadcast ephemeris at seconds from its Toe in WGS-84 ECEF.
std::optional<BroadcastState> propagate_gps_broadcast(const RinexNavRecord& record, double seconds_since_toe);
} // namespace nadir::astro

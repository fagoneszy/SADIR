#pragma once
#include <string>
#include <vector>

namespace nadir::earth {

struct KpSample {
    std::string timestamp;
    double kp{};
    double estimated_kp{};
    int station_count{};
};

struct SolarWindSample {
    std::string timestamp;
    double speed_km_s{};
    double density_p_cm3{};
    double temperature_k{};
};

std::vector<KpSample> parse_noaa_kp(const std::string& text);
std::vector<SolarWindSample> parse_noaa_solar_wind_plasma(const std::string& text);

}

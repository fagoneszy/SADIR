#pragma once
#include <optional>
#include <string>
#include <vector>

namespace nadir::geo {

struct EopRecord {
    double mjd{};
    double xp_arcsec{};
    double yp_arcsec{};
    double dut1_s{};
    double lod_ms{};
    double dx_mas{};
    double dy_mas{};
    bool prediction{};
};

struct EopTable {
    std::vector<EopRecord> records;
    std::string source;
};

std::optional<EopTable> load_iers_csv(const std::string& path);
std::optional<EopRecord> nearest_eop(const EopTable& table,double mjd);
std::optional<EopRecord> interpolate_eop(const EopTable& table,double mjd);

}

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

enum class EopAvailability { Unavailable, Observed, Predicted };

struct EopResolution {
    EopRecord record{};
    EopAvailability availability{EopAvailability::Unavailable};
    double distance_days{};
    constexpr explicit operator bool() const noexcept { return availability != EopAvailability::Unavailable; }
};

std::optional<EopTable> load_iers_csv(const std::string& path);
std::optional<EopRecord> nearest_eop(const EopTable& table,double mjd);
std::optional<EopRecord> interpolate_eop(const EopTable& table,double mjd);
// Resolves only inside the source window. Callers must choose and label any
// fallback outside that window instead of silently using an endpoint.
EopResolution resolve_eop(const EopTable& table,double mjd);

}

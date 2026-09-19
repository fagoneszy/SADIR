#include <nadir/space/ephemeris_provider.hpp>
#include <nadir/data/http.hpp>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace nadir::space {

namespace {
constexpr double pi = 3.141592653589793238462643383279502884;
constexpr double deg_to_rad(double deg) noexcept {
    return deg * pi / 180.0;
}

std::string format_jd(double jd) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << jd;
    return oss.str();
}

std::string utc_ns_to_jd_str(std::int64_t utc_ns) {
    const double jd = 2440587.5 + static_cast<double>(utc_ns) / 86400.0e9;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << jd;
    return oss.str();
}

}

HorizonsEphemerisProvider::HorizonsEphemerisProvider(const std::string& base_url)
    : base_url_(base_url)
{
}

std::string HorizonsEphemerisProvider::build_url(const EphemerisQuery& query) const {
    std::ostringstream oss;
    oss << base_url_ << "?format=text&COMMAND='" << query.horizons_id
        << "'&OBJ_DATA='YES'&MAKE_EPHEM='YES'&EPHEM_TYPE='VECTORS'"
        << "&CENTER='500@10'&START_TIME='" << utc_ns_to_jd_str(query.utc_ns) << "'"
        << "&STOP_TIME='" << utc_ns_to_jd_str(query.utc_ns + 60000000000LL) << "'"
        << "&STEP_SIZE='1 min'&OUT_UNITS='KM-S'&REF_PLANE='ECLIPTIC'"
        << "&VEC_TABLE='2'&CSV_FORMAT='YES'";
    return oss.str();
}

std::optional<BodyState> HorizonsEphemerisProvider::parse_csv_response(const std::string& csv) const {
    const auto soe = csv.find("$$SOE");
    const auto eoe = csv.find("$$EOE");
    if (soe == std::string::npos || eoe == std::string::npos || eoe <= soe) {
        return std::nullopt;
    }

    std::istringstream iss(csv.substr(soe + 5, eoe - soe - 5));
    std::string line;
    if (!std::getline(iss, line)) return std::nullopt;

    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) {
        fields.push_back(field);
    }

    if (fields.size() < 8) return std::nullopt;

    try {
        BodyState state{};
        state.heliocentric_position_km = {
            std::stod(fields[2]),
            std::stod(fields[3]),
            std::stod(fields[4])
        };
        state.heliocentric_velocity_km_s = {
            std::stod(fields[5]),
            std::stod(fields[6]),
            std::stod(fields[7])
        };
        state.distance_from_sun_km = std::sqrt(
            fields[2].empty() ? 0 : std::stod(fields[2]) * std::stod(fields[2]) +
            fields[3].empty() ? 0 : std::stod(fields[3]) * std::stod(fields[3]) +
            fields[4].empty() ? 0 : std::stod(fields[4]) * std::stod(fields[4])
        );
        return state;
    } catch (...) {
        return std::nullopt;
    }
}

EphemerisResult HorizonsEphemerisProvider::fetch(const EphemerisQuery& query) {
    EphemerisResult result{};
    const auto url = build_url(query);

    const auto response = nadir::data::http_get(url);
    if (!response.ok) {
        result.error = "HTTP error: " + response.error;
        return result;
    }

    const auto state = parse_csv_response(response.body);
    if (!state) {
        result.error = "Failed to parse Horizons response";
        return result;
    }

    result.ok = true;
    result.state = *state;
    result.state.horizons_id = query.horizons_id;
    result.state.utc_ns = query.utc_ns;
    return result;
}

std::vector<EphemerisResult> HorizonsEphemerisProvider::fetch_multiple(
    const std::vector<EphemerisQuery>& queries) {
    std::vector<EphemerisResult> results;
    results.reserve(queries.size());
    for (const auto& q : queries) {
        results.push_back(fetch(q));
    }
    return results;
}

} // namespace nadir::space
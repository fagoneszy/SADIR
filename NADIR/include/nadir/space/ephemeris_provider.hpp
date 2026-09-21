#pragma once

#include <nadir/space/body.hpp>
#include <nadir/orbit/numerical.hpp>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace nadir::space {

enum class EphemerisFrame {
    Ecliptic,
    J2000,
    TEME
};

struct EphemerisQuery {
    int horizons_id{};
    std::int64_t utc_ns{};
    EphemerisFrame frame{EphemerisFrame::Ecliptic};
};

struct EphemerisResult {
    bool ok{false};
    std::string error{};
    BodyState state{};
};

class EphemerisProvider {
public:
    virtual ~EphemerisProvider() = default;
    virtual EphemerisResult fetch(const EphemerisQuery& query) = 0;
    virtual std::vector<EphemerisResult> fetch_multiple(
        const std::vector<EphemerisQuery>& queries) = 0;
};

class HorizonsEphemerisProvider : public EphemerisProvider {
public:
    explicit HorizonsEphemerisProvider(const std::string& base_url = "https://ssd.jpl.nasa.gov/api/horizons.api");
    ~HorizonsEphemerisProvider() override = default;

    EphemerisResult fetch(const EphemerisQuery& query) override;
    std::vector<EphemerisResult> fetch_multiple(
        const std::vector<EphemerisQuery>& queries) override;

private:
    std::string base_url_;
    std::string build_url(const EphemerisQuery& query) const;
    std::optional<BodyState> parse_csv_response(const std::string& csv) const;
};

// Converts two heliocentric Horizons-style states to a geocentric third-body
// perturbation. Both states must describe the same epoch; the gravitational
// parameter is intentionally supplied by the caller/source model.
std::optional<orbit::ThirdBody> geocentric_third_body(const BodyState& body,
                                                       const BodyState& earth,
                                                       double mu_m3_s2);

}

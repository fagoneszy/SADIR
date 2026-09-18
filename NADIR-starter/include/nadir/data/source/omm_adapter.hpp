#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <chrono>

#include "nadir/data/source.hpp"
#include "nadir/time/time_state.hpp"
#include "nadir/data/normalized.hpp"

namespace nadir::data {

// ============================================================================
// OMM Adapter - CelesTrak Orbital Mechanics Manual source
// ============================================================================
// Primary format per CelesTrak recommendation (July 2026): OMM JSON/KVN/XML/CSV
// TLE is now secondary/compatibility only due to NORAD ID limit overflow
// ============================================================================

struct OMMElement {
    // Standard OMM/GGP elements
    int norad_id{0};
    std::string international_designator;  // e.g., "2026-0A1A"
    
    // Epoch fields
    int epoch_year{0};
    double epoch_day{0.0};
    
    // Orbital elements (SGP4 theory)
    double inclination{0.0};       // deg
    double raan{0.0};              // Right Ascension of Ascending Node, deg
    double eccentricity{0.0};
    double arg_perigee{0.0};       // Argument of perigee, deg
    double mean_anomaly{0.0};      // Mean anomaly at epoch, deg
    double mean_motion{0.0};       // Revolutions per day
    
    // Derived fields
    double orbit_period_min{0.0};
    double decay_rate{0.0};        // Drag decay rate
    
    // Source metadata
    std::string source{"CELESTRAK"};
    std::string format{"OMM_JSON"};
    int64_t fetched_ns{0};
    std::string checksum{"-"};
};

// ============================================================================
// OMMAdapter - fetches and parses OMM data from CelesTrak
// ============================================================================

class OMMAdapter : public Source {
public:
    OMMAdapter();
    ~OMMAdapter() override = default;

    // Source interface
    std::string id() const override;
    std::chrono::seconds interval() const override;
    bool fetch() override;
    SourceStatus status() const override;
    std::vector<Datum<OMMElement>> get_updates() override;

    // Query by NORAD ID
    std::optional<OMMElement> get_element(int norad_id) const;

    // Set CelesTrak base URL / rate limiting
    void set_base_url(std::string_view url);
    void set_rate_limit_seconds(int seconds);

    // Force refresh (ignores rate limit)
    void force_refresh();

private:
    // Parse a single OMM GP line (JSON format)
    std::optional<OMMElement> parse_omm_json(std::string_view json_data);

    // Parse OMM KVN format (alternative)
    std::optional<OMMElement> parse_omm_knv(std::string_view kvn_data);

    // Parse TLE as fallback (secondary)
    std::optional<OMMElement> parse_tle_fallback(std::string_view line1, std::string_view line2);

    // Normalize OMM element to NADIR Datum
    Datum<OMMElement> normalize_to_datum(const OMMElement& elem);

    // Internal state
    bool online_{false};
    bool stale_{true};
    std::chrono::system_clock::time_point last_fetch_{};
    std::chrono::system_clock::time_point next_allowed_fetch_{};
    
    // Rate limiting: CelesTrak recommends <30 calls/min, ~1 per 2 hours typical
    int rate_limit_seconds_{7200};  // 2 hours default
    std::string base_url_{"https://celestrak.org/NORAD/elements/";
    
    // Cached elements
    std::vector<OMMElement> cached_elements_;
    std::chrono::system_clock::time_point last_rate_check_{};
    
    // Last fetched raw data (for debugging)
    std::string last_raw_data_;
};

// ============================================================================
// Factory function
// ============================================================================

// Create an OMM adapter connected to CelesTrak
OMMAdapter create_omm_adapter();

// Create an OMM adapter with custom base URL
OMMAdapter create_omm_adapter(std::string_view custom_url);

} // namespace nadir::data
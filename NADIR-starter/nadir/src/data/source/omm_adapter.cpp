#include "nadir/data/source/omm_adapter.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <sys/stat.h>

// Helper: check if file exists (for local cache)
static bool file_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// Helper: read entire file content
static std::string read_file(const std::string& path) {
    std::ifstream t(path);
    std::stringstream ss;
    ss << t.rdbuf();
    return ss.str();
}

// Helper: write data to cache file
static bool write_cache(const std::string& path, const std::string& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out << data;
    return true;
}

// ============================================================================
// OMMAdapter implementation
// ============================================================================

namespace nadir::data {

OMMAdapter::OMMAdapter() {
    online_ = false;
    stale_ = true;
    rate_limit_seconds_ = 7200;  // 2 hours, per CelesTrak usage policy
    next_allowed_fetch_ = std::chrono::system_clock::now();
}

std::string OMMAdapter::id() const {
    return "OMM-CelesTrak";
}

std::chrono::seconds OMMAdapter::interval() const {
    return std::chrono::seconds(rate_limit_seconds_);
}

bool OMMAdapter::fetch() {
    auto now = std::chrono::system_clock::now();

    // Rate limiting check
    if (!is_fresh_fetch_allowed(now)) {
        status_.stale = true;
        status_.online = false;
        status_.error = "Rate limit active; next fetch allowed " +
            std::to_string(rate_limit_seconds_ - 
                std::chrono::duration_cast<std::chrono::seconds>(now - last_rate_check_).count()) +
            " seconds ago";
        return false;
    }

    // In a real implementation, we would HTTP fetch from CelesTrak
    // For now, try local cache first, then demonstrate parsing
    
    // Try to read cached OMM data
    std::string cache_path = 
        std::string(get_cache_dir()) / "celestrak_omm_cache.json";
    
    std::string json_data;
    
    if (file_exists(cache_path)) {
        json_data = read_file(cache_path);
        status_.online = true;
    } else {
        // No cache - in production would HTTP fetch from:
        // https://celestrak.org/NORAD/elements/omm.json
        // or specific satellite JSON files
        status_.online = false;
        status_.stale = true;
        status_.error = "No cached OMM data available; "
            "run with network access or provide local cache";
        return false;
    }

    // Parse the OMM JSON data
    parse_omm_data(json_data);
    
    last_fetch_ = now;
    last_rate_check_ = now;
    
    // Update next allowed fetch time
    next_allowed_fetch_ = now + std::chrono::seconds(rate_limit_seconds_);
    
    stale_ = false;
    online_ = true;
    status_.online = true;
    status_.stale = false;
    status_.error.clear();
    status_.last_fetch = now;
    
    return true;
}

bool OMMAdapter::is_fresh_fetch_allowed(std::chrono::system_clock::time_point now) {
    // If enough time has passed since last rate-check
    auto elapsed = now - last_rate_check_;
    if (elapsed.count() >= rate_limit_seconds_) {
        return true;
    }
    // Check if we're past the next allowed fetch
    return now >= next_allowed_fetch_;
}

SourceStatus OMMAdapter::status() const {
    SourceStatus s;
    s.online = online_;
    s.stale = stale_;
    s.last_fetch = last_fetch_;
    s.error = status_.error;
    // Calculate next fetch time
    if (online_) {
        s.next_fetch = next_allowed_fetch_;
    } else {
        s.next_fetch = last_fetch_ + std::chrono::seconds(rate_limit_seconds_);
    }
    return s;
}

std::vector<Datum<OMMElement>> OMMAdapter::get_updates() {
    std::vector<Datum<OMMElement>> updates;
    
    if (cached_elements_.empty()) {
        return updates;
    }
    
    // Return normalized datums for each cached element
    for (const auto& elem : cached_elements_) {
        updates.push_back(normalize_to_datum(elem));
    }
    
    return updates;
}

std::optional<OMMElement> OMMAdapter::get_element(int norad_id) const {
    for (const auto& elem : cached_elements_) {
        if (elem.norad_id == norad_id) {
            return elem;
        }
    }
    return std::nullopt;
}

void OMMAdapter::set_base_url(std::string_view url) {
    base_url_ = std::string(url);
    // Ensure trailing slash if not present
    if (!base_url_.empty() && base_url_.back() != '/') {
        base_url_.push_back('/');
    }
}

void OMMAdapter::set_rate_limit_seconds(int seconds) {
    rate_limit_seconds_ = seconds;
    // If already online, adjust next allowed fetch
    auto now = std::chrono::system_clock::now();
    next_allowed_fetch_ = now + std::chrono::seconds(seconds);
}

void OMMAdapter::force_refresh() {
    // Ignore rate limit, force a fetch
    auto now = std::chrono::system_clock::now();
    last_rate_check_ = now - std::chrono::seconds(rate_limit_seconds_ + 1); // Force refresh
    fetch();
}

std::string OMMAdapter::get_cache_dir() {
    // Platform-specific cache directory
    // Windows: %LOCALAPPDATA%\NADIR\cache
    // Linux: ~/.local/share/NADIR/cache
    // For now, return current directory / .cache
    return ".cache";
}

void OMMAdapter::parse_omm_data(const std::string& json_data) {
    // In a real implementation, we would use a JSON parser (nlohmann/json, rapidjson, etc.)
    // For now, demonstrate the parsing logic with a simplified approach
    
    // The OMM JSON format from CelesTrak contains GP element sets
    // Key fields we need:
    // - NORAD ID
    // - Epoch year/day
    // - Inclination
    // - RAAN
    // - Eccentricity  
    // - Arg perigee
    // - Mean anomaly
    // - Mean motion
    
    // Since we don't have a JSON library compiled, we'll demonstrate the parsing
    // approach with a mock implementation using what we can extract
    
    // For this demo, parse known satellite elements from simple text format
    // that CelesTrak also supports (JSON subset)
    
    // Try to extract elements using simple string search as demonstration
    // Real implementation would use proper JSON parsing
    
    // Mock: parse ISS (NORAD 25544) from demonstration data
    if (json_data.find("25544") != std::string_view::npos ||
        json_data.find("ISS") != std::string_view::npos) {
        
        // Demonstrate parsing a single OMM element
        OMMElement iss;
        iss.norad_id = 25544;
        iss.international_designator = "2026-0A1A";
        iss.epoch_year = 2026;
        iss.epoch_day = 120.5;  // around April 30
        iss.inclination = 51.6;       // deg, ISS inclination
        iss.raan = 38.0;              // deg
        iss.eccentricity = 0.0005;    // very low for ISS
        iss.arg_perigee = 331.0;      // deg
        iss.mean_anomaly = 120.0;     // deg
        iss.mean_motion = 15.71;      // revs per day
        iss.orbit_period_min = 92.6;  // minutes
        iss.decay_rate = 0.0;         // minimal drag
        
        iss.source = "CELESTRAK";
        iss.format = "OMM_JSON";
        iss.checksum = "demo-checksum-iss";
        
        cached_elements_.push_back(iss);
    }
    
    // In a real implementation, we would iterate through all satellites
    // in the JSON array and parse each one
    
    // Also try TLE fallback if OMM parsing yielded nothing
    if (cached_elements_.empty()) {
        // Would call parse_tle_fallback here
    }
    
    // Set fetch timestamp
    fetched_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

std::optional<OMMElement> OMMAdapter::parse_tle_fallback(
    std::string_view line1, std::string_view line2) {
    
    // Minimal TLE parser as fallback (secondary source)
    // This handles the case where OMM data is unavailable
    
    OMMElement elem;
    
    // Validate TLE format
    if (line1.size() < 68 || line2.size() < 68) {
        return std::nullopt;
    }
    
    // Parse NORAD ID from line1
    try {
        elem.norad_id = std::stoi(std::string(line1.substr(2, 5)));
    } catch (...) {
        return std::nullopt;
    }
    
    // Basic validation: first char should be '1' for line1
    if (line1[0] != '1') {
        return std::nullopt;
    }
    
    // Parse epoch year/day from line2
    // Epoch year: characters 18-20 (2-digit year)
    // Epoch day: characters 21-32 (day of year)
    std::string epoch_year_str = std::string(line2.substr(18, 2));
    std::string epoch_day_str = std::string(line2.substr(20, 12));
    
    try {
        int two_digit_year = std::stoi(epoch_year_str);
        // Convert 2-digit year to 4-digit (assume 2000-2099 for recent)
        elem.epoch_year = 2000 + two_digit_year;
        
        // Parse day of year
        double epoch_day_val = std::stod(std::string(epoch_day_str));
        elem.epoch_day = epoch_day_val;
    } catch (...) {
        return std::nullopt;
    }
    
    // Parse orbital elements from line2 positions 26-52
    // Inclination: 8 chars starting at position 26
    try {
        elem.inclination = std::stod(std::string(line2.substr(26, 8)));
    } catch (...) {
        elem.inclination = 0.0;
    }
    
    // RAAN: 8 chars starting at position 34
    try {
        elem.raan = std::stod(std::string(line2.substr(33, 8)));
    } catch (...) {
        elem.raan = 0.0;
    }
    
    // Eccentricity: 7 chars starting at position 42 (preceded by "0.")
    try {
        std::string ecc_str = "0." + std::string(line2.substr(43, 7));
        elem.eccentricity = std::stod(ecc_str);
    } catch (...) {
        elem.eccentricity = 0.0;
    }
    
    // Arg perigee: 8 chars starting at position 50
    try {
        elem.arg_perigee = std::stod(std::string(line2.substr(50, 8)));
    } catch (...) {
        elem.arg_perigee = 0.0;
    }
    
    // Mean anomaly: 8 chars starting at position 58
    try {
        elem.mean_anomaly = std::stod(std::string(line2.substr(63, 8)));
    } catch (...) {
        elem.mean_anomaly = 0.0;
    }
    
    // Mean motion: 11 chars starting at position 64
    try {
        elem.mean_motion = std::stod(std::string(line2.substr(64, 11)));
    } catch (...) {
        elem.mean_motion = 0.0;
    }
    
    elem.orbit_period_min = 60.0 / elem.mean_motion;  // approx, if mean_motion > 0
    elem.source = "CELESTRAK (TLE FALLBACK)";
    elem.format = "TLE";
    elem.checksum = "tle-fallback";
    
    return elem;
}

Datum<OMMElement> OMMAdapter::normalize_to_datum(const OMMElement& elem) {
    Datum<OMMElement> datum;
    datum.value = elem;
    
    // Populate provenance
    datum.provenance.source = elem.source;
    datum.provenance.dataset = "GP/OMM";
    datum.provenance.format = elem.format;
    datum.provenance.checksum = elem.checksum;
    datum.provenance.fetched_ns = elem.fetched_ns;
    datum.provenance.observed_ns = elem.fetched_ns;
    datum.provenance.valid_from_ns = elem.fetched_ns;
    datum.provenance.valid_to_ns = elem.fetched_ns + 86400LL * 1'000'000'000LL;  // 1 day valid
    datum.provenance.uncertainty = 0.0;  // Would calculate from SGP4 theory
    
    // Reference frame
    datum.frame = ReferenceFrame::TEME;  // OMM elements are typically TEME/J2000
    
    return datum;
}

// Factory functions

OMMAdapter create_omm_adapter() {
    return OMMAdapter{};
}

OMMAdapter create_omm_adapter(std::string_view custom_url) {
    auto adapter = OMMAdapter{};
    adapter.set_base_url(custom_url);
    return adapter;
}

} // namespace nadir::data
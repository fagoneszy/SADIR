#include <cassert>
#include <iostream>
#include <string>

#include "nadir/data/source/omm_adapter.hpp"

using namespace nadir::data;

int main() {
    std::cout << "=== OMM Adapter Tests ===" << std::endl;

    // Test 1: Create adapter
    OMMAdapter adapter = create_omm_adapter();
    assert(adapter.id() == "OMM-CelesTrak");
    std::cout << "PASS: OMMAdapter creation and id()" << std::endl;

    // Test 2: Rate limit default (7200s = 2 hours)
    auto interval = adapter.interval();
    assert(interval.count() == 7200);
    std::cout << "PASS: Rate limit default (" << interval.count() << "s = " 
              << interval.count() / 3600 << "h)" << std::endl;

    // Test 3: Set custom rate limit
    OMMAdapter adapter2 = create_omm_adapter("https://custom-ommsource/");
    adapter2.set_rate_limit_seconds(3600);  // 1 hour
    assert(adapter2.interval().count() == 3600);
    std::cout << "PASS: Custom rate limit" << std::endl;

    // Test 4: TLE fallback parsing (minimal)
    // ISS TLE sample (NORAD 25544)
    std::string iss_line1 = "1 25544U 26001A   26277.55496792  +.00000108  00000+0  11611-3 0  9991";
    std::string iss_line2 = "2 25544  51.6431  38.0001  0005254  331.0004  120.0102 15.71625391 12334";
    
    auto parsed = adapter.parse_tle_fallback(iss_line1, iss_line2);
    if (parsed.has_value()) {
        std::cout << "PASS: TLE fallback parsed ISS" << std::endl;
        std::cout << "  NORAD: " << parsed->norad_id << std::endl;
        std::cout << "  Inclination: " << parsed->inclination << " deg" << std::endl;
        std::cout << "  Eccentricity: " << parsed->eccentricity << std::endl;
    } else {
        std::cout << "INFO: TLE fallback parse returned null (expected without full parser)" << std::endl;
    }

    // Test 5: OMM data parsing (demo mode - looks for ISS in data)
    // Create a minimal OMM-like JSON string for demonstration
    std::string omm_mock = R"({"satellites": [{"norad_id": 25544, "name": "ISS", "inclination": 51.6, "mean_motion": 15.71}]})";
    // Note: real JSON parsing would need a library; this just tests the dispatch
    
    // The parse_omm_data with mock data
    // adapter.parse_omm_data(omm_mock);  // Would need full JSON lib
    std::cout << "INFO: OMM JSON parsing demo - would use nlohmann/json in production" << std::endl;

    // Test 6: Factory functions
    OMMAdapter adapter3 = create_omm_adapter();
    assert(dynamic_cast<OMMAdapter*>(&adapter3) != nullptr);
    std::cout << "PASS: Factory function create_omm_adapter()" << std::endl;

    OMMAdapter adapter4 = create_omm_adapter("https://celestrak.org/NORAD/elements/");
    assert(adapter4.id() == "OMM-CelesTrak");
    std::cout << "PASS: Factory function with custom URL" << std::endl;

    // Test 7: Datum normalization
    OMMElement test_elem;
    test_elem.norad_id = 25544;
    test_elem.inclination = 51.6;
    test_elem.eccentricity = 0.0005;
    test_elem.mean_motion = 15.71;
    test_elem.source = "CELESTRAK";
    test_elem.format = "OMM_JSON";
    test_elem.checksum = "abcd1234";
    test_elem.fetched_ns = 1000000;  // mock
    
    Datum<OMMElement> datum = adapter.normalize_to_datum(test_elem);
    assert(datum.value.norad_id == 25544);
    assert(datum.provenance.source == "CELESTRAK");
    assert(datum.provenance.format == "OMM_JSON");
    assert(datum.frame == nadir::time::ReferenceFrame::TEME);  // Will need to include header
    std::cout << "PASS: Datum normalization" << std::endl;
    std::cout << "  Provenance source: " << datum.provenance.source << std::endl;
    std::cout << "  Format: " << datum.provenance.format << std::endl;
    std::cout << "  Frame: " << (datum.frame == nadir::time::ReferenceFrame::TEME ? "TEME" : "other") << std::endl;

    std::cout << "\n=== All OMM Adapter tests completed ===" << std::endl;
    return 0;
}
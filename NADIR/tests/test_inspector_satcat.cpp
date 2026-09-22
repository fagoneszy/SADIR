#include <nadir/data/cache.hpp>
#include <nadir/tui/app.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

int main() {
    const auto root = std::filesystem::temp_directory_path() / "nadir-inspector-satcat";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);
    const auto sources = root / "sources.tsv";
    { std::ofstream out(sources); out << "id\tdomain\tformat\tinterval_seconds\tauth\tsyncable\tauthority\tcoverage\tlicense\tpriority\turl\tdescription\n"
        "celestrak.stations\torbit\tjson\t7200\tnone\t1\tCelesTrak\tEarth\tterms\t1\thttps://example.test/omm\tfixture\n"; }
#ifdef _WIN32
    _putenv_s("NADIR_SOURCES", sources.string().c_str());
    _putenv_s("NADIR_CACHE", (root / "cache").string().c_str());
#else
    setenv("NADIR_SOURCES", sources.string().c_str(), 1);
    setenv("NADIR_CACHE", (root / "cache").string().c_str(), 1);
#endif
    nadir::data::CacheStore store((root / "cache").string());
    const nadir::data::Source omm{"celestrak.stations", "orbit", "json", 0, "none", true, "CelesTrak", "Earth", "terms", 1, "https://example.test/omm", "fixture"};
    const nadir::data::Source satcat{"celestrak.satcat", "orbit", "json", 0, "none", true, "CelesTrak", "Earth", "terms", 1, "https://example.test/satcat", "fixture"};
    const std::string omm_json = "[{\"OBJECT_NAME\":\"ISS\",\"OBJECT_ID\":\"1998-067A\",\"NORAD_CAT_ID\":25544,\"EPOCH\":\"2026-01-01T00:00:00\",\"MEAN_MOTION\":15.5,\"ECCENTRICITY\":0.001,\"INCLINATION\":51.6,\"RA_OF_ASC_NODE\":10,\"ARG_OF_PERICENTER\":20,\"MEAN_ANOMALY\":30,\"BSTAR\":0.0001}]";
    const std::string satcat_json = "[{\"NORAD_CAT_ID\":25544,\"OBJECT_TYPE\":\"PAY\",\"OPS_STATUS_CODE\":\"+\",\"OWNER\":\"US\",\"DECAY_DATE\":\"\"}]";
    if (!store.store(omm, omm_json, 200) || !store.store(satcat, satcat_json, 200)) return 1;
    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    const auto result = nadir::tui::App{}.run({"orbit", "inspect", "stations", "ISS"});
    std::cout.rdbuf(previous);
    std::filesystem::remove_all(root);
    if (result != 0 || output.str().find("TYPE       PAY") == std::string::npos ||
        output.str().find("OPS STATUS +") == std::string::npos || output.str().find("OWNER      US") == std::string::npos ||
        output.str().find("DECAY      NONE") == std::string::npos) return 2;
    return 0;
}

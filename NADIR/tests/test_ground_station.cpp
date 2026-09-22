#include <nadir/geo/station.hpp>
#include <nadir/tui/app.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "nadir-ground-stations.tsv";
    std::ofstream out(path);
    out << "id\tname\tlatitude_deg\tlongitude_deg\taltitude_m\televation_mask_deg\tsource\n";
    out << "BR-TEST\tTest station\t-12.093\t-45.786\t850\t10\tfixture\n";
    out << "BR-TEST\tDuplicate\t0\t0\t0\t0\tfixture\n";
    out << "BAD\tBad\t95\t0\t0\t0\tfixture\n";
    out.close();
    const auto stations = nadir::geo::load_ground_stations_tsv(path.string());
    if (!stations || stations->size() != 1) return 1;
    const auto* station = nadir::geo::find_ground_station(*stations, "BR-TEST");
    if (!station || station->elevation_mask_deg != 10.0 || station->location.altitude_m != 850.0) return 2;
    const auto coops=nadir::geo::parse_noaa_coops_stations_json(R"({"stations":[{"id":"8720218","name":"Mayport","lat":30.398,"lng":-81.427}]})");
    if(!coops||coops->size()!=1||coops->front().source!="NOAA CO-OPS")return 4;
    std::ostringstream console;
    auto* const previous_console = std::cout.rdbuf(console.rdbuf());
    const int catalog_status = nadir::tui::App{}.run({"station", "catalog", path.string()});
    const int info_status = nadir::tui::App{}.run({"station", "info", path.string(), "BR-TEST"});
    std::cout.rdbuf(previous_console);
    std::filesystem::remove(path);
    return catalog_status != 0 || info_status != 0 || console.str().find("STATIONS 1") == std::string::npos ||
        console.str().find("MASK 10") == std::string::npos ? 3 : 0;
}

#include <nadir/geo/station.hpp>

#include <filesystem>
#include <fstream>

int main() {
    const auto path = std::filesystem::temp_directory_path() / "nadir-ground-stations.tsv";
    std::ofstream out(path);
    out << "id\tname\tlatitude_deg\tlongitude_deg\taltitude_m\televation_mask_deg\tsource\n";
    out << "BR-TEST\tTest station\t-12.093\t-45.786\t850\t10\tfixture\n";
    out << "BR-TEST\tDuplicate\t0\t0\t0\t0\tfixture\n";
    out << "BAD\tBad\t95\t0\t0\t0\tfixture\n";
    out.close();
    const auto stations = nadir::geo::load_ground_stations_tsv(path.string());
    std::filesystem::remove(path);
    if (!stations || stations->size() != 1) return 1;
    const auto* station = nadir::geo::find_ground_station(*stations, "BR-TEST");
    return !station || station->elevation_mask_deg != 10.0 || station->location.altitude_m != 850.0 ? 2 : 0;
}

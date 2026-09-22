#include <nadir/astro/omm.hpp>
#include <nadir/data/http.hpp>
#include <nadir/earth/space_weather.hpp>

#include <iostream>

int main() {
    const nadir::data::HttpClient client;
    if (!client.available()) { std::cerr << "curl unavailable\n"; return 1; }
    const auto omm = client.get("https://celestrak.org/NORAD/elements/gp.php?GROUP=STATIONS&FORMAT=JSON");
    if (!omm.ok) { std::cerr << "CelesTrak transport failed: " << omm.error << '\n'; return 2; }
    const auto records = nadir::astro::parse_omm_json(omm.body);
    if (!records.ok || records.records.empty()) { std::cerr << "CelesTrak OMM schema failed\n"; return 3; }
    const auto f107 = client.get("https://services.swpc.noaa.gov/products/10cm-flux-30-day.json");
    if (!f107.ok) { std::cerr << "SWPC transport failed: " << f107.error << '\n'; return 4; }
    const auto samples = nadir::earth::parse_noaa_f107(f107.body);
    if (samples.empty()) { std::cerr << "SWPC F10.7 schema failed\n"; return 5; }
    std::cout << "SOURCE NETWORK PASS OMM=" << records.records.size() << " F107=" << samples.size() << '\n';
    return 0;
}

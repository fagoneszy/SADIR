#include <nadir/astro/omm.hpp>
#include <cstdint>
#include <string>

int main() {
    const char* kvn = "OBJECT_NAME = ISS\nNORAD_CAT_ID = 25544\nEPOCH = 2026-01-01T00:00:00\nMEAN_MOTION = 15.5\nECCENTRICITY = 0.001\nINCLINATION = 51.6\nRA_OF_ASC_NODE = 10\nARG_OF_PERICENTER = 20\nMEAN_ANOMALY = 30\n";
    const auto parsed_kvn = nadir::astro::parse_omm_kvn(kvn);
    if (!parsed_kvn.ok || parsed_kvn.records.size() != 1 || parsed_kvn.records[0].norad_cat_id != 25544) return 1;
    const char* xml = "<omm><OBJECT_NAME>ISS</OBJECT_NAME><NORAD_CAT_ID>25544</NORAD_CAT_ID><EPOCH>2026-01-01T00:00:00</EPOCH><MEAN_MOTION>15.5</MEAN_MOTION><ECCENTRICITY>0.001</ECCENTRICITY><INCLINATION>51.6</INCLINATION><RA_OF_ASC_NODE>10</RA_OF_ASC_NODE><ARG_OF_PERICENTER>20</ARG_OF_PERICENTER><MEAN_ANOMALY>30</MEAN_ANOMALY></omm>";
    const auto parsed_xml = nadir::astro::parse_omm_xml(xml);
    if (!parsed_xml.ok || parsed_xml.records[0].object_name != "ISS") return 2;
    auto with_source = parsed_kvn;
    nadir::astro::attach_source_metadata(with_source, {"celestrak.omm", "https://example.test/omm", "abc", "2026-01-01T00:01:00", "SGP4"});
    if (with_source.records[0].source_id != "celestrak.omm" || with_source.records[0].content_sha256 != "abc") return 3;
    const auto round_trip=nadir::astro::parse_omm_kvn(nadir::astro::write_omm_kvn(parsed_kvn.records.front()));
    if (!round_trip.ok || round_trip.records.front().norad_cat_id != 25544 || round_trip.records.front().object_name != "ISS") return 4;
    if (nadir::astro::parse_omm_kvn("NORAD_CAT_ID = 0").ok) return 5;
    if (nadir::astro::parse_omm_json(std::string(16U * 1024U * 1024U + 1U, ' ')).ok) return 6;
    std::uint32_t random = 0xd6e8feb9U;
    for (int sample = 0; sample < 512; ++sample) {
        const int length = static_cast<int>(random % 512U);
        random = random * 1664525U + 1013904223U;
        std::string fuzz;
        fuzz.reserve(length);
        for (int index = 0; index < length; ++index) {
            random = random * 1664525U + 1013904223U;
            fuzz.push_back(static_cast<char>(random >> 24U));
        }
        (void)nadir::astro::parse_omm_json(fuzz);
        (void)nadir::astro::parse_omm_kvn(fuzz);
        (void)nadir::astro::parse_omm_xml(fuzz);
    }
    return 0;
}

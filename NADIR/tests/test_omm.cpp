#include <nadir/astro/omm.hpp>

int main() {
    const char* kvn = "OBJECT_NAME = ISS\nNORAD_CAT_ID = 25544\nEPOCH = 2026-01-01T00:00:00\nMEAN_MOTION = 15.5\nECCENTRICITY = 0.001\nINCLINATION = 51.6\nRA_OF_ASC_NODE = 10\nARG_OF_PERICENTER = 20\nMEAN_ANOMALY = 30\n";
    const auto parsed_kvn = nadir::astro::parse_omm_kvn(kvn);
    if (!parsed_kvn.ok || parsed_kvn.records.size() != 1 || parsed_kvn.records[0].norad_cat_id != 25544) return 1;
    const char* xml = "<omm><OBJECT_NAME>ISS</OBJECT_NAME><NORAD_CAT_ID>25544</NORAD_CAT_ID><EPOCH>2026-01-01T00:00:00</EPOCH><MEAN_MOTION>15.5</MEAN_MOTION><ECCENTRICITY>0.001</ECCENTRICITY><INCLINATION>51.6</INCLINATION><RA_OF_ASC_NODE>10</RA_OF_ASC_NODE><ARG_OF_PERICENTER>20</ARG_OF_PERICENTER><MEAN_ANOMALY>30</MEAN_ANOMALY></omm>";
    const auto parsed_xml = nadir::astro::parse_omm_xml(xml);
    if (!parsed_xml.ok || parsed_xml.records[0].object_name != "ISS") return 2;
    return nadir::astro::parse_omm_kvn("NORAD_CAT_ID = 0").ok ? 3 : 0;
}

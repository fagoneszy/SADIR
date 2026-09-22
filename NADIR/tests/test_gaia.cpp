#include <nadir/astro/gaia.hpp>
#include <cmath>
#include <string>
int main() {
 const auto parsed=nadir::astro::parse_gaia_csv("source_id,ra,dec,phot_g_mean_mag\n5853498713190525696,201.2983,-11.1613,12.42\n");
 if(!parsed.ok||!parsed.sources||parsed.sources->size()!=1||(*parsed.sources)[0].source_id!=5853498713190525696ULL||std::abs((*parsed.sources)[0].right_ascension_deg-201.2983)>1e-12) return 1;
 if(nadir::astro::parse_gaia_csv("source_id,ra,dec,phot_g_mean_mag\n1,360,0,1\n").ok) return 2;
 return nadir::astro::parse_gaia_csv(std::string(64U*1024U*1024U+1U,'x')).ok?3:0;
}

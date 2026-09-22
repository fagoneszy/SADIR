#include <nadir/astro/gaia.hpp>
#include <nadir/tui/app.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
int main() {
 const auto parsed=nadir::astro::parse_gaia_csv("source_id,ra,dec,phot_g_mean_mag,ref_epoch,parallax,pmra,pmdec\n5853498713190525696,201.2983,-11.1613,12.42,2016.0,2.1,3600,0\n");
 if(!parsed.ok||!parsed.sources||parsed.sources->size()!=1||(*parsed.sources)[0].source_id!=5853498713190525696ULL||std::abs((*parsed.sources)[0].right_ascension_deg-201.2983)>1e-12) return 1;
 const auto projected=nadir::astro::propagate_gaia_linear((*parsed.sources)[0],2026.0);
 if(!projected||std::abs(projected->right_ascension_deg-(201.2983+0.01/std::cos(-11.1613*3.14159265358979323846/180.0)))>1e-12) return 2;
 const auto path=std::filesystem::temp_directory_path()/"nadir-gaia.csv";
 { std::ofstream output(path); output << "source_id,ra,dec,phot_g_mean_mag\n5853498713190525696,201.2983,-11.1613,12.42\n"; output.close(); }
 std::ostringstream console; auto* previous=std::cout.rdbuf(console.rdbuf()); const int status=nadir::tui::App{}.run({"gaia","inspect",path.string(),"2026"}); std::cout.rdbuf(previous); std::filesystem::remove(path);
 if(status!=0) return 3;
 if(console.str().find("SOURCES 1")==std::string::npos) return 4;
 if(console.str().find("EPOCH 2026")==std::string::npos) return 5;
 if(nadir::astro::parse_gaia_csv("source_id,ra,dec,phot_g_mean_mag\n1,360,0,1\n").ok) return 6;
 return nadir::astro::parse_gaia_csv(std::string(64U*1024U*1024U+1U,'x')).ok?7:0;
}

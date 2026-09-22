#include <nadir/astro/gaia.hpp>
#include <cmath>
#include <sstream>
#include <unordered_map>
namespace nadir::astro {
namespace { constexpr std::size_t max_bytes=64U*1024U*1024U, max_lines=1'000'000U;
std::vector<std::string> fields(const std::string& line) { std::vector<std::string> out; std::string value; std::istringstream input(line); while (std::getline(input,value,',')) { if(!value.empty() && value.back()=='\r') value.pop_back(); out.push_back(value); } return out; }
std::optional<double> number(const std::string& value) { try { std::size_t used{}; const auto result=std::stod(value,&used); return used==value.size() && std::isfinite(result)?std::optional{result}:std::nullopt; } catch (...) { return std::nullopt; } }
} // namespace
GaiaParseResult parse_gaia_csv(const std::string& text) {
 if (text.size()>max_bytes) return {false,{},"Gaia CSV exceeds size limit"}; std::istringstream input(text); std::string line;
 if (!std::getline(input,line)) return {false,{},"missing Gaia CSV header"}; const auto header=fields(line); std::unordered_map<std::string,std::size_t> column; for(std::size_t i=0;i<header.size();++i) column[header[i]]=i;
 for (const auto name : {"source_id","ra","dec","phot_g_mean_mag"}) if (!column.contains(name)) return {false,{},"missing Gaia CSV column"};
 std::vector<GaiaSource> result; std::size_t lines{};
 while (std::getline(input,line)) { if (++lines>max_lines) return {false,{},"Gaia CSV has too many lines"}; if (line.empty()) continue; const auto row=fields(line);
  const auto get=[&](const char* name)->const std::string* { const auto found=column.find(name); if(found==column.end()) return nullptr; return found->second<row.size()?&row[found->second]:nullptr; };
  const auto* id_text=get("source_id"); const auto* ra_text=get("ra"); const auto* dec_text=get("dec"); const auto* magnitude_text=get("phot_g_mean_mag"); if(!id_text||!ra_text||!dec_text||!magnitude_text) return {false,{},"truncated Gaia CSV row"};
  const auto optional_number=[&](const char* name)->std::optional<double> { const auto* value=get(name); if(!value || value->empty()) return {}; return number(*value); };
  try { std::size_t used{}; const auto id=std::stoull(*id_text,&used); const auto ra=number(*ra_text), dec=number(*dec_text), magnitude=number(*magnitude_text); const auto epoch=optional_number("ref_epoch"), parallax=optional_number("parallax"), pmra=optional_number("pmra"), pmdec=optional_number("pmdec");
   if (used!=id_text->size() || id==0 || !ra || !dec || !magnitude || (get("ref_epoch") && !get("ref_epoch")->empty() && !epoch) || (get("parallax") && !get("parallax")->empty() && !parallax) || (get("pmra") && !get("pmra")->empty() && !pmra) || (get("pmdec") && !get("pmdec")->empty() && !pmdec) || *ra<0.0 || *ra>=360.0 || *dec<-90.0 || *dec>90.0 || (epoch && *epoch<1900.0)) return {false,{},"invalid Gaia source"}; result.push_back({id,*ra,*dec,*magnitude,epoch.value_or(2016.0),parallax,pmra,pmdec}); } catch (...) { return {false,{},"invalid Gaia source"}; }
 }
 if(result.empty()) return {false,{},"Gaia CSV has no sources"}; return {true,std::move(result),{}};
}
std::optional<GaiaSkyPosition> propagate_gaia_linear(const GaiaSource& source, double epoch_year) noexcept {
 if(!std::isfinite(epoch_year)||!std::isfinite(source.right_ascension_deg)||!std::isfinite(source.declination_deg)||!std::isfinite(source.reference_epoch_year)||source.right_ascension_deg<0.0||source.right_ascension_deg>=360.0||source.declination_deg<-90.0||source.declination_deg>90.0) return std::nullopt;
 if(!source.proper_motion_ra_mas_per_year&&!source.proper_motion_dec_mas_per_year) return GaiaSkyPosition{source.right_ascension_deg,source.declination_deg};
 const double cosine=std::cos(source.declination_deg*3.14159265358979323846/180.0); if(std::abs(cosine)<1.0e-12) return std::nullopt; const double years=epoch_year-source.reference_epoch_year;
 double ra=source.right_ascension_deg+(source.proper_motion_ra_mas_per_year.value_or(0.0)/cosine)*years/3.6e6; const double dec=source.declination_deg+source.proper_motion_dec_mas_per_year.value_or(0.0)*years/3.6e6;
 if(!std::isfinite(ra)||!std::isfinite(dec)||dec<-90.0||dec>90.0) return std::nullopt; ra=std::fmod(ra,360.0); if(ra<0.0) ra+=360.0; return GaiaSkyPosition{ra,dec};
}
} // namespace nadir::astro

#include <nadir/astro/gaia.hpp>
#include <cmath>
#include <sstream>
#include <unordered_map>
namespace nadir::astro {
namespace { constexpr std::size_t max_bytes=64U*1024U*1024U, max_lines=1'000'000U;
std::vector<std::string> fields(const std::string& line) { std::vector<std::string> out; std::string value; std::istringstream input(line); while (std::getline(input,value,',')) out.push_back(value); return out; }
std::optional<double> number(const std::string& value) { try { std::size_t used{}; const auto result=std::stod(value,&used); return used==value.size() && std::isfinite(result)?std::optional{result}:std::nullopt; } catch (...) { return std::nullopt; } }
} // namespace
GaiaParseResult parse_gaia_csv(const std::string& text) {
 if (text.size()>max_bytes) return {false,{},"Gaia CSV exceeds size limit"}; std::istringstream input(text); std::string line;
 if (!std::getline(input,line)) return {false,{},"missing Gaia CSV header"}; const auto header=fields(line); std::unordered_map<std::string,std::size_t> column; for(std::size_t i=0;i<header.size();++i) column[header[i]]=i;
 for (const auto name : {"source_id","ra","dec","phot_g_mean_mag"}) if (!column.contains(name)) return {false,{},"missing Gaia CSV column"};
 std::vector<GaiaSource> result; std::size_t lines{};
 while (std::getline(input,line)) { if (++lines>max_lines) return {false,{},"Gaia CSV has too many lines"}; if (line.empty()) continue; const auto row=fields(line);
  const auto get=[&](const char* name)->const std::string* { const auto index=column[name]; return index<row.size()?&row[index]:nullptr; };
  const auto* id_text=get("source_id"); const auto* ra_text=get("ra"); const auto* dec_text=get("dec"); const auto* magnitude_text=get("phot_g_mean_mag"); if(!id_text||!ra_text||!dec_text||!magnitude_text) return {false,{},"truncated Gaia CSV row"};
  try { std::size_t used{}; const auto id=std::stoull(*id_text,&used); const auto ra=number(*ra_text), dec=number(*dec_text), magnitude=number(*magnitude_text); if (used!=id_text->size() || id==0 || !ra || !dec || !magnitude || *ra<0.0 || *ra>=360.0 || *dec<-90.0 || *dec>90.0) return {false,{},"invalid Gaia source"}; result.push_back({id,*ra,*dec,*magnitude}); } catch (...) { return {false,{},"invalid Gaia source"}; }
 }
 if(result.empty()) return {false,{},"Gaia CSV has no sources"}; return {true,std::move(result),{}};
}
} // namespace nadir::astro

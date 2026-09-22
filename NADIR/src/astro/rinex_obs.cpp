#include <nadir/astro/rinex_obs.hpp>
#include <cmath>
#include <iomanip>
#include <sstream>
namespace nadir::astro { namespace { constexpr std::size_t max_bytes=16U*1024U*1024U,max_lines=1'000'000U; } 
RinexObservationParseResult parse_rinex_observation_epochs(const std::string& text) { if(text.size()>max_bytes)return{false,{},"RINEX observation exceeds size limit"};std::istringstream input(text);std::string line;bool header{},end{};std::size_t lines{};std::vector<RinexObservationEpoch> epochs;
 while(std::getline(input,line)){if(++lines>max_lines)return{false,{},"RINEX observation has too many lines"};if(!header){if(line.find("RINEX VERSION / TYPE")!=std::string::npos)header=true;continue;}if(!end){if(line.find("END OF HEADER")!=std::string::npos)end=true;continue;}if(line.empty())continue;if(line[0]!='>')continue;std::istringstream f(line.substr(1));int y{},mo{},d{},h{},mi{},flag{},count{};double s{};if(!(f>>y>>mo>>d>>h>>mi>>s>>flag>>count)||!std::isfinite(s)||y<1900||mo<1||mo>12||d<1||d>31||h<0||h>23||mi<0||mi>59||s<0.0||s>=61.0||flag<0||flag>6||count<0)return{false,{},"invalid RINEX observation epoch"};std::ostringstream epoch;epoch<<std::setfill('0')<<std::setw(4)<<y<<'-'<<std::setw(2)<<mo<<'-'<<std::setw(2)<<d<<'T'<<std::setw(2)<<h<<':'<<std::setw(2)<<mi<<':'<<std::fixed<<std::setprecision(3)<<std::setw(6)<<s;epochs.push_back({epoch.str(),flag,count});}
 if(!header||!end||epochs.empty())return{false,{},"missing RINEX observation header or epochs"};return{true,std::move(epochs),{}}; }
} // namespace nadir::astro

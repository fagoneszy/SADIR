#include <nadir/astro/rinex_nav.hpp>
#include <array>
#include <cmath>
#include <iomanip>
#include <sstream>
namespace nadir::astro { namespace {
constexpr std::size_t max_bytes=16U*1024U*1024U,max_lines=1'000'000U;
constexpr double mu=3.986005e14,omega_earth=7.2921151467e-5;
std::optional<double> number(std::string v){for(auto& c:v)if(c=='D'||c=='d')c='E';try{std::size_t n{};auto x=std::stod(v,&n);return n==v.size()&&std::isfinite(x)?std::optional{x}:std::nullopt;}catch(...){return{};}}
std::optional<std::array<double,4>> fields(const std::string& line){std::istringstream in(line);std::string text;std::array<double,4> out{};for(auto& value:out){if(!(in>>text))return{};const auto parsed=number(text);if(!parsed)return{};value=*parsed;}return out;}
double wrapped_week(double seconds){while(seconds>302400.0)seconds-=604800.0;while(seconds<-302400.0)seconds+=604800.0;return seconds;}
}
RinexNavParseResult parse_rinex_navigation(const std::string& text){
 if(text.size()>max_bytes)return{false,{},"RINEX navigation exceeds size limit"};std::istringstream input(text);std::string line;bool header{},end{};std::size_t lines{};std::vector<RinexNavRecord> records;
 while(std::getline(input,line)){if(++lines>max_lines)return{false,{},"RINEX navigation has too many lines"};if(!header){if(line.find("RINEX VERSION / TYPE")!=std::string::npos)header=true;continue;}if(!end){if(line.find("END OF HEADER")!=std::string::npos)end=true;continue;}if(line.empty()||line[0]==' ')continue;
  std::istringstream first(line);std::string id,bias,drift,rate;int y{},mo{},d{},h{},mi{};double s{};if(!(first>>id>>y>>mo>>d>>h>>mi>>s>>bias>>drift>>rate))return{false,{},"invalid RINEX navigation record"};auto b=number(bias),dr=number(drift),r=number(rate);if(id.size()<2||!b||!dr||!r||!std::isfinite(s)||mo<1||mo>12||d<1||d>31||h<0||h>23||mi<0||mi>59||s<0.0||s>=61.0)return{false,{},"invalid RINEX navigation record"};
  std::ostringstream epoch;epoch<<std::setfill('0')<<std::setw(4)<<y<<'-'<<std::setw(2)<<mo<<'-'<<std::setw(2)<<d<<'T'<<std::setw(2)<<h<<':'<<std::setw(2)<<mi<<':'<<std::fixed<<std::setprecision(3)<<std::setw(6)<<s;RinexNavRecord record{id,epoch.str(),*b,*dr,*r,{}};
  if(id[0]=='G'&&input.peek()!=std::char_traits<char>::eof()){std::array<std::array<double,4>,7> block{};for(auto& values:block){if(!std::getline(input,line)||++lines>max_lines)return{false,{},"truncated GPS broadcast ephemeris"};auto parsed=fields(line);if(!parsed)return{false,{},"invalid GPS broadcast ephemeris"};values=*parsed;}record.gps=GpsBroadcastEphemeris{block[2][0],block[1][3],block[1][1],block[3][0],block[4][0],block[2][2],block[3][3],block[3][2],block[0][3],block[0][2],block[1][0],block[1][2],block[2][1],block[2][3],block[3][1],block[0][1],block[5][2]};auto& g=*record.gps;if(g.sqrt_a_m_half<=0.0||g.eccentricity<0.0||g.eccentricity>=1.0||!std::isfinite(g.toe_s))return{false,{},"invalid GPS broadcast ephemeris"};}
  records.push_back(std::move(record));}
 if(!header||!end||records.empty())return{false,{},"missing RINEX navigation header or records"};return{true,std::move(records),{}};
}
std::optional<BroadcastState> propagate_gps_broadcast(const RinexNavRecord& record,double seconds_since_toe){
 if(!record.gps||!std::isfinite(seconds_since_toe))return{};const auto& g=*record.gps;const double tk=wrapped_week(seconds_since_toe),a=g.sqrt_a_m_half*g.sqrt_a_m_half,n=std::sqrt(mu/(a*a*a))+g.mean_motion_delta_rad_s;const double mean=g.mean_anomaly_rad+n*tk;double eccentric=mean;for(int i{};i<12;++i){const double step=(eccentric-g.eccentricity*std::sin(eccentric)-mean)/(1.0-g.eccentricity*std::cos(eccentric));eccentric-=step;if(std::abs(step)<1e-13)break;}
 const double anomaly=std::atan2(std::sqrt(1.0-g.eccentricity*g.eccentricity)*std::sin(eccentric),std::cos(eccentric)-g.eccentricity),argument=anomaly+g.argument_of_perigee_rad,twice=2.0*argument,u=argument+g.cus_rad*std::sin(twice)+g.cuc_rad*std::cos(twice),radius=a*(1.0-g.eccentricity*std::cos(eccentric))+g.crs_m*std::sin(twice)+g.crc_m*std::cos(twice),inclination=g.inclination_rad+g.inclination_rate_rad_s*tk+g.cis_rad*std::sin(twice)+g.cic_rad*std::cos(twice),x=radius*std::cos(u),y=radius*std::sin(u),node=g.right_ascension_rad+(g.right_ascension_rate_rad_s-omega_earth)*tk-omega_earth*g.toe_s,clock=record.clock_bias_s+record.clock_drift_s_s*tk+record.clock_drift_rate_s_s2*tk*tk-g.clock_group_delay_s;
 return BroadcastState{{x*std::cos(node)-y*std::cos(inclination)*std::sin(node),x*std::sin(node)+y*std::cos(inclination)*std::cos(node),y*std::sin(inclination)},clock};
}
} // namespace nadir::astro

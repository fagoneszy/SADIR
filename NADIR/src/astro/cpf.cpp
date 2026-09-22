#include <nadir/astro/cpf.hpp>
#include <cmath>
#include <sstream>
namespace nadir::astro {
CpfParseResult parse_cpf(const std::string& text) {
    constexpr std::size_t max_bytes=16U*1024U*1024U,max_lines=1'000'000U;
    if(text.size()>max_bytes)return{false,{},"CPF exceeds size limit"};std::istringstream input(text);std::string line,kind;std::size_t lines{};bool h1{},h2{},h9{},trailer{};std::vector<CpfPosition> positions;
    while(std::getline(input,line)){if(++lines>max_lines)return{false,{},"CPF has too many lines"};std::istringstream fields(line);if(!(fields>>kind)||kind=="00")continue;if(kind=="H1"){h1=true;continue;}if(kind=="H2"){h2=true;continue;}if(kind=="H9"){h9=true;continue;}if(kind=="99"){trailer=true;break;}if(kind!="10")continue;if(!h1||!h2||!h9)return{false,{},"CPF position before required headers"};CpfPosition point;if(!(fields>>point.direction>>point.modified_julian_date>>point.seconds_of_day_utc>>point.leap_second_flag>>point.position_m.x>>point.position_m.y>>point.position_m.z)||point.direction<0||point.direction>2||point.modified_julian_date<0||point.modified_julian_date>1'000'000||point.seconds_of_day_utc<0.0||point.seconds_of_day_utc>86400.0||!std::isfinite(point.seconds_of_day_utc)||!std::isfinite(point.position_m.x)||!std::isfinite(point.position_m.y)||!std::isfinite(point.position_m.z))return{false,{},"invalid CPF position"};positions.push_back(point);}
    if(!h1||!h2||!h9||!trailer||positions.empty())return{false,{},"missing CPF headers, positions, or trailer"};return{true,std::move(positions),{}};
}
} // namespace nadir::astro

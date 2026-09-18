#include <nadir/earth/space_weather.hpp>
#include <nadir/core/json.hpp>
#include <algorithm>
#include <cctype>
#include <map>

namespace nadir::earth {

static std::string lower(std::string s) {
    std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

static int col(const json::Array& h,const std::vector<std::string>& names) {
    for (std::size_t i=0;i<h.size();++i) {
        const auto k=lower(h[i].as_string());
        for (const auto& n:names) if (k==lower(n)) return static_cast<int>(i);
    }
    return -1;
}

static double number(const json::Array& a,int i) {
    if (i<0 || static_cast<std::size_t>(i)>=a.size()) return 0.0;
    if (a[static_cast<std::size_t>(i)].is_number()) return a[static_cast<std::size_t>(i)].as_number();
    try { return std::stod(a[static_cast<std::size_t>(i)].as_string()); } catch (...) { return 0.0; }
}

static std::string string(const json::Array& a,int i) {
    if (i<0 || static_cast<std::size_t>(i)>=a.size()) return {};
    return a[static_cast<std::size_t>(i)].as_string();
}

std::vector<KpSample> parse_noaa_kp(const std::string& text) {
    const auto p=json::parse(text);
    std::vector<KpSample> out;
    const auto* rows=p.ok?p.value.as_array():nullptr;
    if (!rows || rows->empty() || !(*rows)[0].is_array()) return out;
    const auto& h=*(*rows)[0].as_array();
    const int t=col(h,{"time_tag","time"});
    const int kp=col(h,{"Kp","kp_index"});
    const int est=col(h,{"estimated_kp","estimated_kp_index"});
    const int st=col(h,{"station_count"});
    for (std::size_t i=1;i<rows->size();++i) {
        const auto* r=(*rows)[i].as_array();
        if (!r) continue;
        out.push_back({string(*r,t),number(*r,kp),number(*r,est),static_cast<int>(number(*r,st))});
    }
    return out;
}

std::vector<SolarWindSample> parse_noaa_solar_wind_plasma(const std::string& text) {
    const auto p=json::parse(text);
    std::vector<SolarWindSample> out;
    const auto* rows=p.ok?p.value.as_array():nullptr;
    if (!rows || rows->empty() || !(*rows)[0].is_array()) return out;
    const auto& h=*(*rows)[0].as_array();
    const int t=col(h,{"time_tag","time"});
    const int d=col(h,{"density","density_p_cm3"});
    const int s=col(h,{"speed","speed_km_s"});
    const int temp=col(h,{"temperature","temperature_k"});
    for (std::size_t i=1;i<rows->size();++i) {
        const auto* r=(*rows)[i].as_array();
        if (!r) continue;
        out.push_back({string(*r,t),number(*r,s),number(*r,d),number(*r,temp)});
    }
    return out;
}

}

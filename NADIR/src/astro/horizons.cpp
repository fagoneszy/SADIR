#include <nadir/astro/horizons.hpp>
#include <sstream>
#include <vector>

namespace nadir::astro {

static std::vector<std::string> split_csv(const std::string& line) {
    std::vector<std::string> out;
    std::string item;
    bool quoted=false;
    for (char c:line) {
        if (c=='\"') quoted=!quoted;
        else if (c==','&&!quoted) { out.push_back(item); item.clear(); }
        else item+=c;
    }
    out.push_back(item);
    return out;
}

static std::string trim(std::string s) {
    const auto a=s.find_first_not_of(" \t\r\n");
    const auto b=s.find_last_not_of(" \t\r\n");
    if (a==std::string::npos) return {};
    return s.substr(a,b-a+1);
}

std::optional<EphemerisState> parse_horizons_vector_csv(const std::string& response) {
    const auto a=response.find("$$SOE");
    const auto z=response.find("$$EOE");
    if (a==std::string::npos||z==std::string::npos||z<=a) return std::nullopt;
    std::istringstream in(response.substr(a+5,z-a-5));
    std::string line;
    while (std::getline(in,line)) {
        line=trim(line);
        if (line.empty()) continue;
        const auto f=split_csv(line);
        if (f.size()<8) continue;
        try {
            EphemerisState s;
            s.jd=std::stod(trim(f[0]));
            const double x=std::stod(trim(f[2]));
            const double y=std::stod(trim(f[3]));
            const double z=std::stod(trim(f[4]));
            const double vx=std::stod(trim(f[5]));
            const double vy=std::stod(trim(f[6]));
            const double vz=std::stod(trim(f[7]));
            s.position_km={x,y,z};
            s.velocity_km_s={vx,vy,vz};
            const double range=std::sqrt(x*x+y*y+z*z);
            s.range_km=range;
            s.range_rate_km_s=(x*vx+y*vy+z*vz)/range;
            constexpr double c_km_s=299792.458;
            s.light_time_s=range/c_km_s;
            if (f.size()>=11) {
                s.light_time_s=std::stod(trim(f[8]));
                s.range_km=std::stod(trim(f[9]));
                s.range_rate_km_s=std::stod(trim(f[10]));
            }
            return s;
        } catch (...) {}
    }
    return std::nullopt;
}

}

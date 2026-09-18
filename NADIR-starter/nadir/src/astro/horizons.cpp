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
        if (f.size()<11) continue;
        try {
            EphemerisState s;
            s.jd=std::stod(trim(f[0]));
            s.position_km={std::stod(trim(f[2])),std::stod(trim(f[3])),std::stod(trim(f[4]))};
            s.velocity_km_s={std::stod(trim(f[5])),std::stod(trim(f[6])),std::stod(trim(f[7]))};
            s.light_time_s=std::stod(trim(f[8]));
            s.range_km=std::stod(trim(f[9]));
            s.range_rate_km_s=std::stod(trim(f[10]));
            return s;
        } catch (...) {}
    }
    return std::nullopt;
}

}

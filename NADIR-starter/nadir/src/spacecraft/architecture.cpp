#include <nadir/spacecraft/architecture.hpp>
#include <algorithm>
#include <cctype>

namespace nadir::spacecraft {

static std::string low(std::string s) {
    std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
    return s;
}

std::vector<Architecture> public_architectures() {
    const std::string tech="https://starlink.com/technology";
    const std::string v3="https://starlink.com/updates/starlink-version-3-satellites";
    return {
        {"Starlink","V2",{
            {"structure","form factor","compact flat-panel spacecraft",tech},
            {"communications","optical inter-satellite links","3 lasers, up to 200 Gbps each",tech},
            {"communications","user antennas","5 Ku-band phased arrays",tech},
            {"communications","backhaul antennas","3 dual-band Ka/E antennas",tech},
            {"propulsion","thrusters","argon ion propulsion",tech},
            {"power","solar arrays","dual solar arrays",tech},
            {"power","storage","high-capacity battery",tech},
            {"guidance","star tracker","custom star tracker",tech}
        }},
        {"Starlink","V3",{
            {"communications","optical inter-satellite links","6 lasers, 400 Gbps each",v3},
            {"communications","RF backhaul","4 quad-band Ka/E/V/W antennas",v3},
            {"communications","RF backhaul capacity","up to 1.2 Tbps",v3},
            {"power","solar generation","approximately 2x V2 array power",v3},
            {"power","solar blanket","four stitched 19 m segments per complete array",v3}
        }}
    };
}

std::optional<Architecture> find_architecture(const std::string& family,const std::string& generation) {
    for (const auto& a:public_architectures()) if (low(a.family)==low(family)&&low(a.generation)==low(generation)) return a;
    return std::nullopt;
}

}

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
        }},
        {"Webb","JWST",{
            {"structure","observatory mass","approximately 6200 kg","https://science.nasa.gov/mission/webb/fact-sheet/"},
            {"optics","primary mirror","6.5 m, 18 gold-coated beryllium segments","https://science.nasa.gov/mission/webb/fact-sheet/"},
            {"thermal","sunshield","5 layers, 21.197 m x 14.162 m","https://science.nasa.gov/mission/webb/webbs-sunshield/"},
            {"instruments","science instruments","NIRCam, NIRSpec, MIRI, FGS/NIRISS","https://science.nasa.gov/mission/webb/fact-sheet/"},
            {"architecture","main elements","telescope, instrument module, sunshield, spacecraft bus","https://www.nasa.gov/missions/webb/webb-conversations-components-structure-of-nasas-james-webb-space-telescope/"},
            {"guidance","attitude control","reaction wheels, star trackers, gyroscopes, FGS and fine steering mirror","https://science.nasa.gov/mission/webb/faqs-full/"}
        }},
        {"Hubble","HST",{
            {"structure","length","13.2 m","https://science.nasa.gov/mission/hubble/overview/about-hubble/"},
            {"structure","mass","approximately 12200 kg post-SM4","https://science.nasa.gov/mission/hubble/overview/hubble-by-the-numbers/"},
            {"optics","primary mirror","2.4 m","https://science.nasa.gov/mission/hubble/overview/about-hubble/"},
            {"power","solar arrays","greater than 5200 W generated","https://science.nasa.gov/mission/hubble/overview/hubble-by-the-numbers/"},
            {"power","batteries","six batteries, greater than 500 Ah total","https://science.nasa.gov/mission/hubble/overview/hubble-by-the-numbers/"},
            {"guidance","actuators","four reaction wheels and four magnetic torquer bars","https://science.nasa.gov/mission/hubble/overview/about-hubble/"},
            {"guidance","sensors","gyroscopes, Sun sensors, magnetometers, star trackers and fine guidance sensors","https://science.nasa.gov/mission/hubble/overview/about-hubble/"}
        }},
        {"Voyager","V1V2",{
            {"structure","bus","approximately 1.8 m diameter electronics bus","https://science.nasa.gov/mission/voyager/frequently-asked-questions/"},
            {"communications","high-gain antenna","3.7 m diameter","https://science.nasa.gov/mission/voyager/frequently-asked-questions/"},
            {"power","source","three radioisotope thermoelectric generators","https://science.nasa.gov/mission/voyager/spacecraft/"},
            {"compute","command computer subsystem","command decoding, fault response, pointing and sequencing","https://science.nasa.gov/mission/voyager/spacecraft/"},
            {"guidance","AACS","three-axis stabilized attitude and articulation control","https://science.nasa.gov/mission/voyager/spacecraft/"},
            {"communications","radio","S-band uplink and X-band downlink","https://science.nasa.gov/mission/voyager/spacecraft/"}
        }},
        {"ISS","Current",{
            {"structure","pressurized module length","67 m","https://www.nasa.gov/international-space-station/space-station-facts-and-figures/"},
            {"structure","truss length","94 m","https://www.nasa.gov/international-space-station/space-station-facts-and-figures/"},
            {"structure","mass","approximately 419725 kg; varies with visiting vehicles","https://www.nasa.gov/international-space-station/space-station-facts-and-figures/"},
            {"power","solar arrays","8 main arrays, 75 to 90 kW","https://www.nasa.gov/international-space-station/space-station-facts-and-figures/"},
            {"software","flight software","approximately 1.5 million lines","https://www.nasa.gov/international-space-station/space-station-facts-and-figures/"},
            {"network","US segment","44 computers and 100 data networks","https://www.nasa.gov/international-space-station/space-station-facts-and-figures/"}
        }}
    };
}

std::optional<Architecture> find_architecture(const std::string& family,const std::string& generation) {
    for (const auto& a:public_architectures()) if (low(a.family)==low(family)&&low(a.generation)==low(generation)) return a;
    return std::nullopt;
}

}

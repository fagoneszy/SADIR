#include <nadir/astro/targets.hpp>
#include <algorithm>
#include <cctype>

namespace nadir::astro {

static std::string lower(std::string s) {
    std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

const std::vector<SolarTarget>& solar_targets() {
    static const std::vector<SolarTarget> v{
        {"Sun",10,"star","Solar System"},
        {"Mercury",199,"planet","Sun"},
        {"Venus",299,"planet","Sun"},
        {"Earth",399,"planet","Sun"},
        {"Moon",301,"moon","Earth"},
        {"Mars",499,"planet","Sun"},
        {"Phobos",401,"moon","Mars"},
        {"Deimos",402,"moon","Mars"},
        {"Jupiter",599,"planet","Sun"},
        {"Io",501,"moon","Jupiter"},
        {"Europa",502,"moon","Jupiter"},
        {"Ganymede",503,"moon","Jupiter"},
        {"Callisto",504,"moon","Jupiter"},
        {"Amalthea",505,"moon","Jupiter"},
        {"Saturn",699,"planet","Sun"},
        {"Mimas",601,"moon","Saturn"},
        {"Enceladus",602,"moon","Saturn"},
        {"Tethys",603,"moon","Saturn"},
        {"Dione",604,"moon","Saturn"},
        {"Rhea",605,"moon","Saturn"},
        {"Titan",606,"moon","Saturn"},
        {"Hyperion",607,"moon","Saturn"},
        {"Iapetus",608,"moon","Saturn"},
        {"Phoebe",609,"moon","Saturn"},
        {"Uranus",799,"planet","Sun"},
        {"Ariel",701,"moon","Uranus"},
        {"Umbriel",702,"moon","Uranus"},
        {"Titania",703,"moon","Uranus"},
        {"Oberon",704,"moon","Uranus"},
        {"Miranda",705,"moon","Uranus"},
        {"Neptune",899,"planet","Sun"},
        {"Triton",801,"moon","Neptune"},
        {"Nereid",802,"moon","Neptune"},
        {"Proteus",808,"moon","Neptune"},
        {"Pluto",999,"dwarf-planet","Sun"},
        {"Charon",901,"moon","Pluto"},
        {"Nix",902,"moon","Pluto"},
        {"Hydra",903,"moon","Pluto"},
        {"Kerberos",904,"moon","Pluto"},
        {"Styx",905,"moon","Pluto"}
    };
    return v;
}

std::optional<SolarTarget> find_solar_target(const std::string& name) {
    const auto q=lower(name);
    for (const auto& t:solar_targets()) if (lower(t.name)==q || std::to_string(t.horizons_id)==q) return t;
    return std::nullopt;
}

std::vector<SolarTarget> search_solar_targets(const std::string& query) {
    const auto q=lower(query);
    std::vector<SolarTarget> out;
    for (const auto& t:solar_targets()) {
        const auto h=lower(t.name+" "+t.type+" "+t.primary+" "+std::to_string(t.horizons_id));
        if (q.empty() || h.find(q)!=std::string::npos) out.push_back(t);
    }
    return out;
}

}

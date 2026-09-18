#include <nadir/astro/bodies.hpp>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace nadir::astro {

const std::vector<BodyInfo>& bodies() {
    static const std::vector<BodyInfo> v{
        {"Mercury",199,2440.53,2439.4,0.330103,5.4289,58.6462,0.2408467,3.70,4.25},
        {"Venus",299,6051.8,6051.8,4.86731,5.243,-243.018,0.61519726,8.87,10.36},
        {"Earth",399,6378.1366,6371.0084,5.97217,5.5134,0.99726968,1.0000174,9.80,11.19},
        {"Moon",301,1738.1,1737.4,0.07346,3.344,27.321661,0.0748,1.62,2.38},
        {"Mars",499,3396.19,3389.50,0.641691,3.9340,1.02595676,1.8808476,3.71,5.03},
        {"Jupiter",599,71492,69911,1898.125,1.3262,0.41354,11.862615,24.79,60.20},
        {"Saturn",699,60268,58232,568.317,0.6871,0.44401,29.447498,10.44,36.09},
        {"Uranus",799,25559,25362,86.8099,1.270,-0.71833,84.016846,8.87,21.38},
        {"Neptune",899,24764,24622,102.4092,1.638,0.67125,164.79132,11.15,23.56},
        {"Pluto",999,1188.3,1188.3,0.0130246,1.853,-6.3872,247.92065,0.62,1.21}
    };
    return v;
}

static std::string lower(std::string s) {
    std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
    return s;
}

std::optional<BodyInfo> find_body(const std::string& name) {
    const auto n=lower(name);
    for (const auto& b:bodies()) if (lower(b.name)==n) return b;
    return std::nullopt;
}

static std::string encode(const std::string& s) {
    std::ostringstream out;
    out<<std::hex<<std::uppercase;
    for (unsigned char c:s) {
        if (std::isalnum(c)||c=='-'||c=='_'||c=='.'||c=='~') out<<c;
        else out<<'%'<<std::setw(2)<<std::setfill('0')<<static_cast<int>(c);
    }
    return out.str();
}

std::string horizons_vectors_url(int target_id,const std::string& start,const std::string& stop) {
    std::ostringstream q;
    q<<"https://ssd.jpl.nasa.gov/api/horizons.api?format=text";
    q<<"&COMMAND="<<encode("'"+std::to_string(target_id)+"'");
    q<<"&OBJ_DATA="<<encode("'YES'");
    q<<"&MAKE_EPHEM="<<encode("'YES'");
    q<<"&EPHEM_TYPE="<<encode("'VECTORS'");
    q<<"&CENTER="<<encode("'500@399'");
    q<<"&START_TIME="<<encode("'"+start+"'");
    q<<"&STOP_TIME="<<encode("'"+stop+"'");
    q<<"&STEP_SIZE="<<encode("'1 min'");
    q<<"&OUT_UNITS="<<encode("'KM-S'");
    q<<"&REF_PLANE="<<encode("'FRAME'");
    q<<"&VEC_TABLE="<<encode("'2'");
    q<<"&CSV_FORMAT="<<encode("'YES'");
    return q.str();
}

}

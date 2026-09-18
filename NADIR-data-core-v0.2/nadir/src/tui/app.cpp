#include <nadir/tui/app.hpp>
#include <nadir/astro/bodies.hpp>
#include <nadir/astro/horizons.hpp>
#include <nadir/core/terminal.hpp>
#include <nadir/core/time.hpp>
#include <nadir/data/http.hpp>
#include <nadir/data/source.hpp>
#include <nadir/data/sync.hpp>
#include <nadir/geo/wgs84.hpp>
#include <nadir/render/earth.hpp>
#include <nadir/render/framebuffer.hpp>
#include <nadir/satellite/tle.hpp>
#include <nadir/spacecraft/architecture.hpp>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace nadir::tui {

static std::vector<std::string> split(const std::string& line) {
    std::istringstream ss(line);
    std::vector<std::string> out;
    std::string token;
    while (ss >> token) out.push_back(token);
    return out;
}

static std::string source_file() {
    if (const char* p=std::getenv("NADIR_SOURCES")) return p;
    return "config/sources.tsv";
}

static std::string cache_root() {
    if (const char* p=std::getenv("NADIR_CACHE")) return p;
    return "data/cache";
}

static std::string utc_minute(std::chrono::system_clock::time_point tp) {
    const auto t=std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
#ifdef _WIN32
    gmtime_s(&tm,&t);
#else
    gmtime_r(&t,&tm);
#endif
    std::ostringstream out;
    out<<std::put_time(&tm,"%Y-%m-%d %H:%M");
    return out.str();
}

int App::run(const std::vector<std::string>& args) {
    if (args.empty()) return shell();
    return command(args);
}

int App::shell() {
    Terminal::clear();
    banner();
    std::string line;
    while (true) {
        std::cout << "\x1b[38;2;0;255;119mNADIR>\x1b[0m " << std::flush;
        if (!std::getline(std::cin, line)) break;
        auto args = split(line);
        if (args.empty()) continue;
        if (args[0] == "exit" || args[0] == "quit") break;
        command(args);
    }
    return 0;
}

int App::command(const std::vector<std::string>& args) {
    if (args[0] == "earth") return earth(args);
    if (args[0] == "geo") return geo(args);
    if (args[0] == "tle") return tle(args);
    if (args[0] == "sources") return sources(args);
    if (args[0] == "sync") return sync(args);
    if (args[0] == "body") return body(args);
    if (args[0] == "solar") return solar(args);
    if (args[0] == "craft") return craft(args);
    if (args[0] == "time") return clock(args);
    if (args[0] == "clear") {
        Terminal::clear();
        return 0;
    }
    if (args[0] == "help") {
        help();
        return 0;
    }
    std::cout << "UNKNOWN COMMAND: " << args[0] << "\n";
    return 1;
}

int App::earth(const std::vector<std::string>& args) {
    render::EarthView view{};
    for (std::size_t i = 1; i + 1 < args.size(); ++i) {
        if (args[i] == "--yaw") view.yaw_deg = std::stod(args[++i]);
        else if (args[i] == "--pitch") view.pitch_deg = std::stod(args[++i]);
        else if (args[i] == "--lat") view.observer_lat_deg = std::stod(args[++i]);
        else if (args[i] == "--lon") view.observer_lon_deg = std::stod(args[++i]);
    }
    const auto ts = Terminal::size();
    const int cols = std::clamp(ts.columns, 60, 180);
    const int rows = std::clamp(ts.rows - 8, 18, 70);
    render::Framebuffer fb(cols * 2, rows * 4);
    render::draw_earth(fb, view);
    std::cout << "\x1b[38;2;0;255;119mEARTH REFERENCE / WGS84\x1b[0m\n";
    std::cout << "LAT " << std::fixed << std::setprecision(6) << view.observer_lat_deg << "  LON " << view.observer_lon_deg << "\n";
    std::cout << fb.braille();
    return 0;
}

int App::geo(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        std::cout << "geo <lat_deg> <lon_deg> <alt_m>\n";
        return 1;
    }
    geo::Geodetic g{std::stod(args[1]), std::stod(args[2]), std::stod(args[3])};
    const auto e = geo::geodetic_to_ecef(g);
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "ECEF X " << e.x << " m\n";
    std::cout << "ECEF Y " << e.y << " m\n";
    std::cout << "ECEF Z " << e.z << " m\n";
    return 0;
}

int App::tle(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        std::cout << "tle <name> <line1> <line2>\n";
        return 1;
    }
    const auto t = satellite::parse_tle(args[1], args[2], args[3]);
    if (!t) {
        std::cout << "INVALID TLE\n";
        return 1;
    }
    std::cout << "OBJECT " << t->name << "\n";
    std::cout << "NORAD  " << t->norad_id << "\n";
    std::cout << "INC    " << t->inclination_deg << " deg\n";
    std::cout << "RAAN   " << t->raan_deg << " deg\n";
    std::cout << "ECC    " << t->eccentricity << "\n";
    std::cout << "MM     " << t->mean_motion_rev_per_day << " rev/day\n";
    return 0;
}

int App::sources(const std::vector<std::string>& args) {
    data::SourceCatalog cat;
    if (!cat.load(source_file())) {
        std::cout<<"SOURCE CATALOG NOT FOUND: "<<source_file()<<"\n";
        return 1;
    }
    const auto list=args.size()>1?cat.domain(args[1]):cat.all();
    std::cout<<std::left<<std::setw(30)<<"ID"<<std::setw(12)<<"DOMAIN"<<std::setw(10)<<"FORMAT"<<std::setw(10)<<"AUTH"<<"SYNC\n";
    for (const auto& s:list) {
        std::cout<<std::left<<std::setw(30)<<s.id<<std::setw(12)<<s.domain<<std::setw(10)<<s.format<<std::setw(10)<<s.auth<<(s.syncable?"YES":"QUERY")<<"\n";
    }
    return 0;
}

int App::sync(const std::vector<std::string>& args) {
    if (args.size()!=2) {
        std::cout<<"sync <source-id|domain:name>\n";
        return 1;
    }
    data::SourceCatalog cat;
    if (!cat.load(source_file())) {
        std::cout<<"SOURCE CATALOG NOT FOUND\n";
        return 1;
    }
    data::SyncEngine engine(cache_root());
    std::vector<data::Source> targets;
    const std::string prefix="domain:";
    if (args[1].rfind(prefix,0)==0) targets=cat.domain(args[1].substr(prefix.size()));
    else {
        const auto s=cat.find(args[1]);
        if (!s) { std::cout<<"SOURCE NOT FOUND\n"; return 1; }
        targets.push_back(*s);
    }
    int failed=0;
    for (const auto& s:targets) {
        if (!s.syncable) continue;
        std::cout<<"SYNC "<<s.id<<" ... "<<std::flush;
        const auto r=engine.fetch(s);
        if (!r.ok) {
            ++failed;
            std::cout<<"FAILED "<<r.error<<"\n";
            continue;
        }
        if (r.skipped) {
            std::cout<<"CACHED "<<r.path<<"\n";
            continue;
        }
        std::cout<<"OK "<<r.bytes<<" B "<<r.sha256.substr(0,16)<<" "<<r.path<<"\n";
    }
    return failed?1:0;
}

int App::body(const std::vector<std::string>& args) {
    if (args.size()<3) {
        std::cout<<"body info <name>\nbody live <name>\n";
        return 1;
    }
    const auto b=astro::find_body(args[2]);
    if (!b) { std::cout<<"BODY NOT FOUND\n"; return 1; }
    if (args[1]=="info") {
        std::cout<<std::fixed<<std::setprecision(6);
        std::cout<<"BODY        "<<b->name<<"\n";
        std::cout<<"HORIZONS    "<<b->horizons_id<<"\n";
        std::cout<<"REQ RADIUS  "<<b->equatorial_radius_km<<" km\n";
        std::cout<<"MEAN RADIUS "<<b->mean_radius_km<<" km\n";
        std::cout<<"MASS        "<<b->mass_1e24kg<<" x10^24 kg\n";
        std::cout<<"DENSITY     "<<b->density_g_cm3<<" g/cm^3\n";
        std::cout<<"ROTATION    "<<b->rotation_days<<" d\n";
        std::cout<<"ORBIT       "<<b->orbital_years<<" y\n";
        std::cout<<"GRAVITY     "<<b->gravity_m_s2<<" m/s^2\n";
        std::cout<<"ESCAPE      "<<b->escape_km_s<<" km/s\n";
        return 0;
    }
    if (args[1]=="live") {
        const auto now=std::chrono::system_clock::now();
        const auto start=utc_minute(now);
        const auto stop=utc_minute(now+std::chrono::minutes(2));
        const auto url=astro::horizons_vectors_url(b->horizons_id,start,stop);
        data::HttpClient http;
        const auto r=http.get(url);
        if (!r.ok) { std::cout<<"HORIZONS FAILED "<<r.error<<"\n"; return 1; }
        const auto state=astro::parse_horizons_vector_csv(r.body);
        if (!state) { std::cout<<r.body<<"\n"; return 0; }
        std::cout<<std::fixed<<std::setprecision(6);
        std::cout<<"BODY       "<<b->name<<"\n";
        std::cout<<"JD TDB     "<<state->jd<<"\n";
        std::cout<<"X          "<<state->position_km.x<<" km\n";
        std::cout<<"Y          "<<state->position_km.y<<" km\n";
        std::cout<<"Z          "<<state->position_km.z<<" km\n";
        std::cout<<"VX         "<<state->velocity_km_s.x<<" km/s\n";
        std::cout<<"VY         "<<state->velocity_km_s.y<<" km/s\n";
        std::cout<<"VZ         "<<state->velocity_km_s.z<<" km/s\n";
        std::cout<<"DISTANCE   "<<state->range_km<<" km\n";
        std::cout<<"RANGE RATE "<<state->range_rate_km_s<<" km/s\n";
        std::cout<<"LIGHT TIME "<<state->light_time_s<<" s\n";
        return 0;
    }
    return 1;
}

int App::solar(const std::vector<std::string>& args) {
    if (args.size()!=2 || args[1]!="live") {
        std::cout<<"solar live\n";
        return 1;
    }
    const auto now=std::chrono::system_clock::now();
    const auto start=utc_minute(now);
    const auto stop=utc_minute(now+std::chrono::minutes(2));
    data::HttpClient http;
    std::cout<<std::left<<std::setw(12)<<"BODY"<<std::right<<std::setw(20)<<"DISTANCE KM"<<std::setw(18)<<"RATE KM/S"<<"\n";
    for (const auto& b:astro::bodies()) {
        if (b.name=="Earth") continue;
        const auto r=http.get(astro::horizons_vectors_url(b.horizons_id,start,stop));
        if (!r.ok) {
            std::cout<<std::left<<std::setw(12)<<b.name<<"FAILED\n";
            continue;
        }
        const auto state=astro::parse_horizons_vector_csv(r.body);
        if (!state) {
            std::cout<<std::left<<std::setw(12)<<b.name<<"PARSE FAILED\n";
            continue;
        }
        std::cout<<std::left<<std::setw(12)<<b.name<<std::right<<std::setw(20)<<std::fixed<<std::setprecision(0)<<state->range_km<<std::setw(18)<<std::setprecision(6)<<state->range_rate_km_s<<"\n";
    }
    return 0;
}

int App::craft(const std::vector<std::string>& args) {
    if (args.size()!=3) {
        std::cout<<"craft <family> <generation>\n";
        return 1;
    }
    const auto a=spacecraft::find_architecture(args[1],args[2]);
    if (!a) { std::cout<<"ARCHITECTURE NOT FOUND\n"; return 1; }
    std::cout<<"SPACECRAFT "<<a->family<<" "<<a->generation<<"\n";
    for (const auto& c:a->components) std::cout<<std::left<<std::setw(16)<<c.subsystem<<std::setw(28)<<c.name<<c.value<<"\n";
    return 0;
}

int App::clock(const std::vector<std::string>&) {
    const auto t=core::now_utc();
    std::cout<<std::fixed<<std::setprecision(9);
    std::cout<<"UTC "<<core::iso8601_utc(t.unix_ns)<<"\n";
    std::cout<<"JD  "<<t.jd_utc<<"\n";
    std::cout<<"MJD "<<t.mjd_utc<<"\n";
    return 0;
}

void App::banner() const {
    std::cout << "\x1b[38;2;0;255;119m";
    std::cout << "NADIR SYSTEMS CONSOLE\n";
    std::cout << "NAVIGATIONAL & ASTRONOMICAL DATA INSTRUMENTATION RENDERER\n";
    std::cout << "BUILD 000002 / C++23\n\n";
    std::cout << "CORE   ONLINE\n";
    std::cout << "WGS84  ONLINE\n";
    std::cout << "RENDER ONLINE\n";
    std::cout << "DATA   ONLINE\n";
    std::cout << "ORBIT  INGEST READY\n";
    std::cout << "NET    PLANNED\n\n";
    std::cout << "TYPE help FOR COMMANDS\n\x1b[0m";
}

void App::help() const {
    std::cout << "earth [--yaw deg] [--pitch deg] [--lat deg] [--lon deg]\n";
    std::cout << "geo <lat_deg> <lon_deg> <alt_m>\n";
    std::cout << "tle <name> <line1> <line2>\n";
    std::cout << "sources [domain]\n";
    std::cout << "sync <source-id|domain:name>\n";
    std::cout << "body info <name>\n";
    std::cout << "body live <name>\n";
    std::cout << "solar live\n";
    std::cout << "craft <family> <generation>\n";
    std::cout << "time\n";
    std::cout << "clear\n";
    std::cout << "exit\n";
}

}

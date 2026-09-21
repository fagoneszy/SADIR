#include <nadir/tui/app.hpp>
#include <nadir/astro/bodies.hpp>
#include <nadir/astro/horizons.hpp>
#include <nadir/astro/omm.hpp>
#include <nadir/astro/targets.hpp>
#include <nadir/core/terminal.hpp>
#include <nadir/core/json.hpp>
#include <nadir/core/time.hpp>
#include <nadir/data/cache.hpp>
#include <nadir/data/http.hpp>
#include <nadir/data/preset.hpp>
#include <nadir/data/source.hpp>
#include <nadir/data/sync.hpp>
#include <nadir/earth/fireball.hpp>
#include <nadir/earth/seismic.hpp>
#include <nadir/earth/space_weather.hpp>
#include <nadir/format/ndr.hpp>
#include <nadir/geo/eop.hpp>
#include <nadir/geo/wgs84.hpp>
#include <nadir/orbit/pass_predictor.hpp>
#include <nadir/orbit/tracker.hpp>
#include <nadir/orbit/scene_adapter.hpp>
#include <nadir/render/earth.hpp>
#include <nadir/render/earth_mesh.hpp>
#include <nadir/render/framebuffer.hpp>
#include <nadir/render/frame_clock.hpp>
#include <nadir/render/phosphor_buffer.hpp>
#include <nadir/render/presenter.hpp>
#include <nadir/render/renderer3d.hpp>
#include <nadir/render/scene_builder.hpp>
#include <nadir/satellite/tle.hpp>
#include <nadir/spacecraft/architecture.hpp>
#include <nadir/system/station.hpp>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

namespace nadir::tui {

static std::vector<std::string> split(const std::string& line) {
    std::istringstream ss(line);
    std::vector<std::string> out;
    std::string token;
    while (ss>>token) out.push_back(token);
    return out;
}

static std::string source_file() {
    if (const char* p=std::getenv("NADIR_SOURCES")) return p;
    return "config/sources.tsv";
}

static std::string preset_file() {
    if (const char* p=std::getenv("NADIR_PRESETS")) return p;
    return "config/presets.tsv";
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

static std::string human_bytes(std::uint64_t n) {
    const char* units[]={"B","KiB","MiB","GiB","TiB"};
    double v=static_cast<double>(n);
    std::size_t i=0;
    while (v>=1024.0 && i<4) { v/=1024.0; ++i; }
    std::ostringstream out;
    out<<std::fixed<<std::setprecision(i?2:0)<<v<<' '<<units[i];
    return out.str();
}

static std::optional<data::SourceCatalog> catalog() {
    data::SourceCatalog c;
    if (!c.load(source_file())) return std::nullopt;
    return c;
}

static std::string orbital_source(std::string s) {
    if (s.rfind("celestrak.",0)==0 || s.rfind("spacetrack.",0)==0) return s;
    return "celestrak."+s;
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
        std::cout<<"\x1b[38;2;0;255;119mNADIR>\x1b[0m "<<std::flush;
        if (!std::getline(std::cin,line)) break;
        auto args=split(line);
        if (args.empty()) continue;
        if (args[0]=="exit" || args[0]=="quit") break;
        command(args);
    }
    return 0;
}

int App::command(const std::vector<std::string>& args) {
    if (args[0]=="earth") return earth(args);
    if (args[0]=="geo") return geo(args);
    if (args[0]=="tle") return tle(args);
    if (args[0]=="sources") return sources(args);
    if (args[0]=="source") return source(args);
    if (args[0]=="presets") return presets(args);
    if (args[0]=="sync") return sync(args);
    if (args[0]=="cache") return cache(args);
    if (args[0]=="ndr") return ndr(args);
    if (args[0]=="body") return body(args);
    if (args[0]=="targets") return targets(args);
    if (args[0]=="target") return target(args);
    if (args[0]=="solar") return solar(args);
    if (args[0]=="craft") return craft(args);
    if (args[0]=="time") return clock(args);
    if (args[0]=="eop") return eop(args);
    if (args[0]=="orbit") return orbit(args);
    if (args[0]=="seismic") return seismic(args);
    if (args[0]=="spaceweather") return spaceweather(args);
    if (args[0]=="fireballs") return fireballs(args);
    if (args[0]=="station") return station(args);
    if (args[0]=="universe") return universe(args);
    if (args[0]=="clear") { Terminal::clear(); return 0; }
    if (args[0]=="help") { help(); return 0; }
    std::cout<<"UNKNOWN COMMAND: "<<args[0]<<"\n";
    return 1;
}

int App::earth(const std::vector<std::string>& args) {
    if (args.size() >= 2 && args[1] == "live") {
        return earth_live(args);
    }

    render::EarthView view{};
    for (std::size_t i=1;i+1<args.size();++i) {
        if (args[i]=="--yaw") view.yaw_deg=std::stod(args[++i]);
        else if (args[i]=="--pitch") view.pitch_deg=std::stod(args[++i]);
        else if (args[i]=="--lat") view.observer_lat_deg=std::stod(args[++i]);
        else if (args[i]=="--lon") view.observer_lon_deg=std::stod(args[++i]);
    }
    const auto ts=Terminal::size();
    const int cols=std::clamp(ts.columns,60,180);
    const int rows=std::clamp(ts.rows-8,18,70);
    render::Framebuffer fb(cols*2,rows*4);
    render::draw_earth(fb,view);
    std::cout<<"\x1b[38;2;0;255;119mEARTH REFERENCE / WGS84\x1b[0m\n";
    std::cout<<"LAT "<<std::fixed<<std::setprecision(6)<<view.observer_lat_deg<<"  LON "<<view.observer_lon_deg<<"\n";
    std::cout<<fb.braille();
    return 0;
}

int App::earth_live(const std::vector<std::string>& args) {
    TerminalSession terminal;
    if (!terminal.valid()) {
        std::cout << "earth live requires an interactive VT terminal\n";
        return 1;
    }
    const auto epoch = time::TimeInstant{0, 0};
    const auto frame_ref = frames::itrf2020();
    const auto origin = state::earth_center();
    render::SceneBuilder builder{epoch, frame_ref, origin};
    const state::TrackedState earth{399, {{}, {}, epoch, frame_ref, origin},
                                    state::StateKind::Simulated, state::StateQuality::Nominal};
    if (!builder.add_object(earth)) return 1;
    const bool show_iss=std::find(args.begin(),args.end(),"--iss")!=args.end();
    if (show_iss) {
        data::CacheStore store(cache_root());
        const auto path=store.latest("celestrak.stations");
        const auto parsed=path?astro::load_omm_json(*path):astro::OmmParseResult{};
        const auto iss=parsed.ok?astro::find_omm(parsed.records,"25544",1):std::vector<astro::OmmRecord>{};
        geo::EopRecord eop{};
        if (const auto ep=store.latest("iers.eop.rapid")) if (const auto table=geo::load_iers_csv(*ep)) {
            if (const auto value=geo::resolve_eop(*table,core::now_utc().mjd_utc)) eop=value.record;
        }
        if (!iss.empty()) {
            const auto now=core::now_utc(eop.dut1_s);
            const auto result=orbit::track_omm({iss.front(),(now.jd_utc-orbit::omm_epoch_jd_utc(iss.front()))*1440.0,now.jd_utc,eop,{},{},0.0});
            if (orbit::add_tracking_scene_object(builder,iss.front().norad_cat_id,result,epoch)) {
                const math::Vec3d position{result.itrf_m.position.x,result.itrf_m.position.y,result.itrf_m.position.z};
                builder.add_point({2,position,1.0f});
                builder.add_label({2,position,"ISS",10});
            }
        }
    }
    const auto mesh = render::generate_earth_mesh(12, 24);
    for (const auto& edge : mesh.lines)
        builder.add_polyline({1, {mesh.vertices[edge[0]], mesh.vertices[edge[1]]}, 0.70f});
    const auto scene = builder.build();
    if (!scene.valid()) return 1;

    auto size = terminal.size();
    int cols = std::clamp(size.columns, 60, 180);
    int rows = std::clamp(size.rows, 22, 70);
    render::Renderer3D renderer(cols * 2, (rows - 4) * 4);
    renderer.set_ellipsoid_occlusion({true, {}, render::wgs84_ellipsoid(), 1});
    render::Presenter presenter(cols, rows);
    render::FrameClock clock(30.0);
    render::Camera camera{};
    camera.distance = 3.5;
    camera.near_plane = 0.10;
    camera.far_plane = 100.0;
    render::DisplayTransform display{};
    bool quit = false;
    int frame = 0;
    double yaw = 0.0;
    while (!quit) {
        const double dt = clock.tick();
        const auto input = terminal.poll_input();
        quit = input.quit;
        yaw += dt * 0.20;
        if (input.left) yaw -= dt * 0.75;
        if (input.right) yaw += dt * 0.75;
        if (input.up) camera.pitch(dt * 0.50);
        if (input.down) camera.pitch(-dt * 0.50);
        if (input.zoom_in) camera.zoom(-dt);
        if (input.zoom_out) camera.zoom(dt);
        camera.yaw(yaw);
        yaw = 0.0;
        size = terminal.size();
        cols = std::clamp(size.columns, 60, 180);
        rows = std::clamp(size.rows, 22, 70);
        renderer.resize(cols * 2, (rows - 4) * 4);
        presenter.resize(cols, rows);
        renderer.render(scene, camera, display, dt);
        render::HUDState hud{};
        hud.fps = clock.target_fps(); hud.frame = frame++; hud.frame_mode = "DEMO ITRF2020 WGS84";
        hud.camera_mode = show_iss?"ORBIT ITRF / ISS CACHE":"ORBIT SYNTHETIC"; hud.stars = false; hud.grid = true; hud.entities = static_cast<int>(scene.objects.size());
        hud.utc = utc_minute(std::chrono::system_clock::now());
        presenter.render(renderer.phosphor(), hud);
        presenter.present(terminal);
        clock.wait();
    }
    return 0;
}

int App::geo(const std::vector<std::string>& args) {
    if (args.size()!=4) { std::cout<<"geo <lat_deg> <lon_deg> <alt_m>\n"; return 1; }
    geo::Geodetic g{std::stod(args[1]),std::stod(args[2]),std::stod(args[3])};
    const auto e=geo::geodetic_to_ecef(g);
    std::cout<<std::fixed<<std::setprecision(3);
    std::cout<<"ECEF X "<<e.x<<" m\nECEF Y "<<e.y<<" m\nECEF Z "<<e.z<<" m\n";
    return 0;
}

int App::tle(const std::vector<std::string>& args) {
    if (args.size()!=4) { std::cout<<"tle <name> <line1> <line2>\n"; return 1; }
    const auto t=satellite::parse_tle(args[1],args[2],args[3]);
    if (!t) { std::cout<<"INVALID TLE\n"; return 1; }
    std::cout<<"OBJECT "<<t->name<<"\nNORAD  "<<t->norad_id<<"\nINC    "<<t->inclination_deg<<" deg\nRAAN   "<<t->raan_deg<<" deg\nECC    "<<t->eccentricity<<"\nMM     "<<t->mean_motion_rev_per_day<<" rev/day\n";
    return 0;
}

int App::sources(const std::vector<std::string>& args) {
    const auto c=catalog();
    if (!c) { std::cout<<"SOURCE CATALOG NOT FOUND: "<<source_file()<<"\n"; return 1; }
    std::vector<data::Source> list;
    if (args.size()>2 && args[1]=="search") list=c->search(args[2]);
    else if (args.size()>1 && args[1]!="stats") list=c->domain(args[1]);
    else list=c->all();
    if (args.size()>1 && args[1]=="stats") {
        std::map<std::string,int> domains;
        std::map<std::string,int> authorities;
        int syncable=0;
        for (const auto& s:c->all()) { ++domains[s.domain]; ++authorities[s.authority]; if (s.syncable) ++syncable; }
        std::cout<<"TOTAL      "<<c->all().size()<<"\nSYNCABLE   "<<syncable<<"\nQUERY/AUTH "<<(c->all().size()-syncable)<<"\n";
        for (const auto& [d,n]:domains) std::cout<<std::left<<std::setw(12)<<d<<n<<"\n";
        return 0;
    }
    std::cout<<std::left<<std::setw(31)<<"ID"<<std::setw(11)<<"DOMAIN"<<std::setw(8)<<"AUTH"<<std::setw(5)<<"P"<<"DESCRIPTION\n";
    for (const auto& s:list) std::cout<<std::left<<std::setw(31)<<s.id<<std::setw(11)<<s.domain<<std::setw(8)<<s.auth<<std::setw(5)<<s.priority<<s.description<<"\n";
    return 0;
}

int App::source(const std::vector<std::string>& args) {
    if (args.size()!=2) { std::cout<<"source <id>\n"; return 1; }
    const auto c=catalog();
    if (!c) return 1;
    const auto s=c->find(args[1]);
    if (!s) { std::cout<<"SOURCE NOT FOUND\n"; return 1; }
    std::cout<<"ID        "<<s->id<<"\nDOMAIN    "<<s->domain<<"\nAUTHORITY "<<s->authority<<"\nCOVERAGE  "<<s->coverage<<"\nFORMAT    "<<s->format<<"\nAUTH      "<<s->auth<<"\nSYNCABLE  "<<(s->syncable?"YES":"NO")<<"\nINTERVAL  "<<s->interval_seconds<<" s\nLICENSE   "<<s->license<<"\nPRIORITY  "<<s->priority<<"\nURL       "<<s->url<<"\nDESC      "<<s->description<<"\n";
    return 0;
}


int App::presets(const std::vector<std::string>& args) {
    data::PresetCatalog p;
    if (!p.load(preset_file())) { std::cout<<"PRESET CATALOG NOT FOUND: "<<preset_file()<<"\n"; return 1; }
    if (args.size()==2) {
        const auto x=p.find(args[1]);
        if (!x) { std::cout<<"PRESET NOT FOUND\n"; return 1; }
        std::cout<<"PRESET "<<x->id<<"\nDESC   "<<x->description<<"\n";
        for (const auto& id:x->source_ids) std::cout<<"  "<<id<<"\n";
        return 0;
    }
    std::cout<<std::left<<std::setw(18)<<"ID"<<std::setw(9)<<"SOURCES"<<"DESCRIPTION\n";
    for (const auto& x:p.all()) std::cout<<std::left<<std::setw(18)<<x.id<<std::setw(9)<<x.source_ids.size()<<x.description<<"\n";
    return 0;
}

int App::sync(const std::vector<std::string>& args) {
    if (args.size()!=2) { std::cout<<"sync <source-id|domain:name|preset:name>\n"; return 1; }
    const auto c=catalog();
    if (!c) { std::cout<<"SOURCE CATALOG NOT FOUND\n"; return 1; }
    data::SyncEngine engine(cache_root());
    std::vector<data::Source> targets;
    const std::string domain_prefix="domain:";
    const std::string preset_prefix="preset:";
    if (args[1].rfind(domain_prefix,0)==0) targets=c->domain(args[1].substr(domain_prefix.size()));
    else if (args[1].rfind(preset_prefix,0)==0) {
        data::PresetCatalog p;
        if (!p.load(preset_file())) { std::cout<<"PRESET CATALOG NOT FOUND\n"; return 1; }
        const auto preset=p.find(args[1].substr(preset_prefix.size()));
        if (!preset) { std::cout<<"PRESET NOT FOUND\n"; return 1; }
        for (const auto& id:preset->source_ids) {
            const auto s=c->find(id);
            if (s) targets.push_back(*s);
        }
    } else {
        const auto s=c->find(args[1]);
        if (!s) { std::cout<<"SOURCE NOT FOUND\n"; return 1; }
        targets.push_back(*s);
    }
    int failed=0;
    for (const auto& s:targets) {
        if (!s.syncable) continue;
        std::cout<<"SYNC "<<s.id<<" ... "<<std::flush;
        const auto r=engine.fetch(s);
        if (!r.ok) { ++failed; std::cout<<"FAILED "<<r.error<<"\n"; continue; }
        if (r.skipped) { std::cout<<"CACHED "<<r.path<<"\n"; continue; }
        std::cout<<"OK "<<r.bytes<<" B "<<r.sha256.substr(0,16)<<" "<<r.path<<"\n";
    }
    return failed?1:0;
}

int App::cache(const std::vector<std::string>& args) {
    if (args.size()!=2) { std::cout<<"cache <source-id>\n"; return 1; }
    data::CacheStore store(cache_root());
    const auto i=store.info(args[1]);
    if (!i) { std::cout<<"NO CACHE\n"; return 1; }
    const auto now=core::now_utc().unix_ns;
    const double age=static_cast<double>(now-i->fetched_unix_ns)/1.0e9;
    const bool valid=store.verify(args[1]);
    std::cout<<"SOURCE  "<<i->source_id<<"\nFETCHED "<<core::iso8601_utc(i->fetched_unix_ns)<<"\nAGE     "<<std::fixed<<std::setprecision(1)<<age<<" s\nBYTES   "<<i->bytes<<"\nSHA256  "<<i->sha256<<"\nVERIFY  "<<(valid?"PASS":"FAIL")<<"\nSTATUS  "<<i->http_status<<"\nPATH    "<<i->data_path<<"\nURL     "<<i->url<<"\n";
    return valid?0:2;
}

int App::ndr(const std::vector<std::string>& args) {
    if (args.size() < 3 || args.size() > 4 || (args[1] != "inspect" && args[1] != "seek")) {
        std::cout << "ndr inspect <path>\nndr seek <path> <timestamp_ns>\n";
        return 1;
    }
    const auto file = format::read_ndr(args[2]);
    if (!file) { std::cout << "NDR READ FAILED\n"; return 1; }
    if (args[1] == "inspect") {
        if (args.size() != 3) { std::cout << "ndr inspect <path>\n"; return 1; }
        std::cout << "NDR V" << file->header.version << "\nTYPE " << file->header.type
                  << "\nTIMESTAMP " << file->header.timestamp_ns << "\nRECORDS " << file->records.size()
                  << "\nCRC32 " << file->header.crc32 << "\n";
        return 0;
    }
    if (args.size() != 4) { std::cout << "ndr seek <path> <timestamp_ns>\n"; return 1; }
    std::uint64_t timestamp{};
    try { timestamp = std::stoull(args[3]); } catch (...) { std::cout << "INVALID TIMESTAMP\n"; return 1; }
    const format::NdrReplay replay{*file};
    const auto* record = replay.seek(timestamp);
    if (!record) { std::cout << "NO RECORD AT OR BEFORE " << timestamp << "\n"; return 1; }
    std::cout << "TIMESTAMP " << record->timestamp_ns << "\nTYPE " << record->type
              << "\nPAYLOAD " << record->payload.size() << "\n";
    switch (static_cast<format::NdrRecordType>(record->type)) {
    case format::NdrRecordType::Source:
        if (const auto source = format::decode_source_block(record->payload))
            std::cout << "SOURCE " << source->source_id << "\nSHA256 " << source->sha256 << "\nBYTES " << source->bytes << "\n";
        break;
    case format::NdrRecordType::Object:
        if (const auto object = format::decode_object_block(record->payload))
            std::cout << "OBJECT " << object->object_id << "\nCATALOG " << object->catalog_id << "\nNAME " << object->name << "\n";
        break;
    case format::NdrRecordType::State:
        if (const auto state = format::decode_state_block(record->payload))
            std::cout << "OBJECT " << state->object_id << "\nX " << state->state.position_m.x << "\nY " << state->state.position_m.y
                      << "\nZ " << state->state.position_m.z << "\n";
        break;
    case format::NdrRecordType::Event:
        break;
    }
    return 0;
}

int App::body(const std::vector<std::string>& args) {
    if (args.size()<3) { std::cout<<"body info <name>\nbody live <name>\n"; return 1; }
    const auto b=astro::find_body(args[2]);
    if (!b) { std::cout<<"BODY NOT FOUND\n"; return 1; }
    if (args[1]=="info") {
        std::cout<<std::fixed<<std::setprecision(6);
        std::cout<<"BODY        "<<b->name<<"\nHORIZONS    "<<b->horizons_id<<"\nREQ RADIUS  "<<b->equatorial_radius_km<<" km\nMEAN RADIUS "<<b->mean_radius_km<<" km\nMASS        "<<b->mass_1e24kg<<" x10^24 kg\nDENSITY     "<<b->density_g_cm3<<" g/cm^3\nROTATION    "<<b->rotation_days<<" d\nORBIT       "<<b->orbital_years<<" y\nGRAVITY     "<<b->gravity_m_s2<<" m/s^2\nESCAPE      "<<b->escape_km_s<<" km/s\n";
        return 0;
    }
    if (args[1]=="live") {
        const auto now=std::chrono::system_clock::now();
        const auto start=utc_minute(now);
        const auto stop=utc_minute(now+std::chrono::minutes(2));
        data::HttpClient http;
        const auto r=http.get(astro::horizons_vectors_url(b->horizons_id,start,stop));
        if (!r.ok) { std::cout<<"HORIZONS FAILED "<<r.error<<"\n"; return 1; }
        const auto state=astro::parse_horizons_vector_csv(r.body);
        if (!state) { std::cout<<"HORIZONS PARSE FAILED\n"; return 1; }
        std::cout<<std::fixed<<std::setprecision(6);
        std::cout<<"BODY       "<<b->name<<"\nJD TDB     "<<state->jd<<"\nX          "<<state->position_km.x<<" km\nY          "<<state->position_km.y<<" km\nZ          "<<state->position_km.z<<" km\nVX         "<<state->velocity_km_s.x<<" km/s\nVY         "<<state->velocity_km_s.y<<" km/s\nVZ         "<<state->velocity_km_s.z<<" km/s\nDISTANCE   "<<state->range_km<<" km\nRANGE RATE "<<state->range_rate_km_s<<" km/s\nLIGHT TIME "<<state->light_time_s<<" s\n";
        return 0;
    }
    return 1;
}


int App::targets(const std::vector<std::string>& args) {
    const auto list=astro::search_solar_targets(args.size()>1?args[1]:"");
    std::cout<<std::left<<std::setw(16)<<"NAME"<<std::setw(12)<<"HORIZONS"<<std::setw(16)<<"TYPE"<<"PRIMARY\n";
    for (const auto& t:list) std::cout<<std::left<<std::setw(16)<<t.name<<std::setw(12)<<t.horizons_id<<std::setw(16)<<t.type<<t.primary<<"\n";
    return 0;
}

int App::target(const std::vector<std::string>& args) {
    if (args.size()!=3 || args[1]!="live") { std::cout<<"target live <name|Horizons-ID>\n"; return 1; }
    const auto t=astro::find_solar_target(args[2]);
    if (!t) { std::cout<<"TARGET NOT FOUND\n"; return 1; }
    const auto now=std::chrono::system_clock::now();
    const auto start=utc_minute(now);
    const auto stop=utc_minute(now+std::chrono::minutes(2));
    data::HttpClient http;
    const auto r=http.get(astro::horizons_vectors_url(t->horizons_id,start,stop));
    if (!r.ok) { std::cout<<"HORIZONS FAILED "<<r.error<<"\n"; return 1; }
    const auto state=astro::parse_horizons_vector_csv(r.body);
    if (!state) { std::cout<<"HORIZONS PARSE FAILED\n"; return 1; }
    std::cout<<std::fixed<<std::setprecision(6);
    std::cout<<"TARGET     "<<t->name<<"\nTYPE       "<<t->type<<"\nPRIMARY    "<<t->primary<<"\nHORIZONS   "<<t->horizons_id<<"\nJD TDB     "<<state->jd<<"\nX          "<<state->position_km.x<<" km\nY          "<<state->position_km.y<<" km\nZ          "<<state->position_km.z<<" km\nVX         "<<state->velocity_km_s.x<<" km/s\nVY         "<<state->velocity_km_s.y<<" km/s\nVZ         "<<state->velocity_km_s.z<<" km/s\nDISTANCE   "<<state->range_km<<" km\nRANGE RATE "<<state->range_rate_km_s<<" km/s\nLIGHT TIME "<<state->light_time_s<<" s\n";
    return 0;
}

int App::solar(const std::vector<std::string>& args) {
    if (args.size()!=2 || args[1]!="live") { std::cout<<"solar live\n"; return 1; }
    const auto now=std::chrono::system_clock::now();
    const auto start=utc_minute(now);
    const auto stop=utc_minute(now+std::chrono::minutes(2));
    data::HttpClient http;
    std::cout<<std::left<<std::setw(12)<<"BODY"<<std::right<<std::setw(20)<<"DISTANCE KM"<<std::setw(18)<<"RATE KM/S"<<"\n";
    for (const auto& b:astro::bodies()) {
        if (b.name=="Earth") continue;
        const auto r=http.get(astro::horizons_vectors_url(b.horizons_id,start,stop));
        if (!r.ok) { std::cout<<std::left<<std::setw(12)<<b.name<<"FAILED\n"; continue; }
        const auto state=astro::parse_horizons_vector_csv(r.body);
        if (!state) { std::cout<<std::left<<std::setw(12)<<b.name<<"PARSE FAILED\n"; continue; }
        std::cout<<std::left<<std::setw(12)<<b.name<<std::right<<std::setw(20)<<std::fixed<<std::setprecision(0)<<state->range_km<<std::setw(18)<<std::setprecision(6)<<state->range_rate_km_s<<"\n";
    }
    return 0;
}

int App::craft(const std::vector<std::string>& args) {
    if (args.size()!=3) { std::cout<<"craft <family> <generation>\n"; return 1; }
    const auto a=spacecraft::find_architecture(args[1],args[2]);
    if (!a) { std::cout<<"ARCHITECTURE NOT FOUND\n"; return 1; }
    std::cout<<"SPACECRAFT "<<a->family<<" "<<a->generation<<"\n";
    for (const auto& c:a->components) std::cout<<std::left<<std::setw(16)<<c.subsystem<<std::setw(28)<<c.name<<c.value<<"\n";
    return 0;
}

int App::clock(const std::vector<std::string>&) {
    const auto t=core::now_utc();
    std::cout<<std::fixed<<std::setprecision(9);
    std::cout<<"UTC       "<<core::iso8601_utc(t.unix_ns)<<"\nJD UTC    "<<t.jd_utc<<"\nMJD UTC   "<<t.mjd_utc<<"\nTAI-UTC   "<<t.tai_minus_utc_s<<" s\nJD TAI    "<<t.jd_tai<<"\nJD TT     "<<t.jd_tt<<"\nJD UT1    "<<t.jd_ut1<<"\n";
    return 0;
}

int App::eop(const std::vector<std::string>& args) {
    const std::string id="iers.eop.rapid";
    data::CacheStore store(cache_root());
    if (args.size()>1 && args[1]=="live") {
        const auto c=catalog();
        if (!c) return 1;
        const auto s=c->find(id);
        if (!s) return 1;
        const auto r=data::SyncEngine(cache_root()).fetch(*s);
        if (!r.ok) { std::cout<<"EOP SYNC FAILED "<<r.error<<"\n"; return 1; }
    }
    const auto path=store.latest(id);
    if (!path) { std::cout<<"NO EOP CACHE. RUN: nadir eop live\n"; return 1; }
    const auto table=geo::load_iers_csv(*path);
    if (!table) { std::cout<<"EOP PARSE FAILED "<<*path<<"\n"; return 1; }
    const auto t0=core::now_utc();
    const auto r=geo::interpolate_eop(*table,t0.mjd_utc);
    if (!r) return 1;
    const auto t=core::now_utc(r->dut1_s);
    std::cout<<std::fixed<<std::setprecision(9);
    std::cout<<"MJD      "<<r->mjd<<"\nXP       "<<r->xp_arcsec<<" arcsec\nYP       "<<r->yp_arcsec<<" arcsec\nDUT1     "<<r->dut1_s<<" s\nLOD      "<<r->lod_ms<<" ms\ndX       "<<r->dx_mas<<" mas\ndY       "<<r->dy_mas<<" mas\nQUALITY  "<<(r->prediction?"PREDICTED":"OBSERVED/RAPID")<<"\nJD UT1   "<<t.jd_ut1<<"\n";
    return 0;
}

int App::orbit(const std::vector<std::string>& args) {
    if (args.size()<3 || (args[1]!="list" && args[1]!="live" && args[1]!="inspect")) { std::cout<<"orbit list <source|group> [query] [limit]\norbit live <source|group> [query] [limit]\norbit inspect <source|group> <query> [lat lon alt_m frequency_hz]\n"; return 1; }
    const auto id=orbital_source(args[2]);
    const auto c=catalog();
    if (!c) return 1;
    const auto s=c->find(id);
    if (!s) { std::cout<<"ORBIT SOURCE NOT FOUND\n"; return 1; }
    data::CacheStore store(cache_root());
    if (args[1]=="live") {
        const auto r=data::SyncEngine(cache_root()).fetch(*s);
        if (!r.ok) { std::cout<<"ORBIT SYNC FAILED "<<r.error<<"\n"; return 1; }
    }
    const auto path=store.latest(id);
    if (!path) { std::cout<<"NO ORBIT CACHE. RUN: nadir orbit live "<<args[2]<<"\n"; return 1; }
    auto parsed=astro::load_omm_json(*path);
    if (!parsed.ok) { std::cout<<"OMM PARSE FAILED "<<parsed.error<<"\n"; return 1; }
    const auto cache_info=store.info(id);
    if (cache_info) astro::attach_source_metadata(parsed, {id, cache_info->url, cache_info->sha256, core::iso8601_utc(cache_info->fetched_unix_ns), "Vallado SGP4/WGS-72"});
    if (args[1]=="inspect") {
        if (args.size()<4) { std::cout<<"orbit inspect <source|group> <query> [lat lon alt_m frequency_hz]\n"; return 1; }
        const auto matches=astro::find_omm(parsed.records,args[3],1);
        if (matches.empty()) { std::cout<<"OBJECT NOT FOUND\n"; return 1; }
        geo::Geodetic observer{};
        double frequency=0.0;
        try {
            if (args.size()>4) observer.latitude_deg=std::stod(args[4]);
            if (args.size()>5) observer.longitude_deg=std::stod(args[5]);
            if (args.size()>6) observer.altitude_m=std::stod(args[6]);
            if (args.size()>7) frequency=std::stod(args[7]);
        } catch (...) { std::cout<<"INVALID OBSERVER OR FREQUENCY\n"; return 1; }
        geo::EopRecord eop{}; std::string eop_quality="FALLBACK ZERO-EOP";
        if (const auto eop_path=store.latest("iers.eop.rapid")) {
            if (const auto table=geo::load_iers_csv(*eop_path)) {
                const auto now=core::now_utc();
                const auto current=geo::resolve_eop(*table,now.mjd_utc);
                if (current) { eop=current.record; eop_quality=eop.prediction?"IERS PREDICTED":"IERS OBSERVED/RAPID"; }
            }
        }
        const auto now=core::now_utc(eop.dut1_s);
        const double source_age_s=cache_info ? std::max(0.0, static_cast<double>(now.unix_ns-cache_info->fetched_unix_ns)/1.0e9) : -1.0;
        const bool source_stale=source_age_s >= 0.0 && s->interval_seconds > 0 && source_age_s > 2.0 * s->interval_seconds;
        const std::string quality=source_stale ? "STALE" : !cache_info ? "INVALID" :
            eop_quality=="FALLBACK ZERO-EOP" ? "ESTIMATED" : eop.prediction ? "PREDICTED" : "PROPAGATED";
        const auto epoch_jd=orbit::omm_epoch_jd_utc(matches.front());
        if (epoch_jd==0.0) { std::cout<<"INVALID OMM EPOCH\n"; return 1; }
        const double minutes=(now.jd_utc-epoch_jd)*1440.0;
        const orbit::TrackingRequest request{matches.front(),minutes,now.jd_utc,eop,observer,{},frequency};
        const auto tracked=orbit::track_omm(request);
        if (!tracked) { std::cout<<"SGP4 FAILED "<<orbit::to_string(tracked.error)<<"\n"; return 1; }
        const auto elevation=[&](double minute) -> std::optional<double> {
            auto next=request; next.minutes_since_epoch=minute; next.jd_utc=epoch_jd+minute/1440.0;
            const auto state=orbit::track_omm(next); return state ? std::optional<double>{state.topocentric.elevation_deg} : std::nullopt;
        };
        const auto passes=orbit::predict_passes(elevation,minutes,minutes+1440.0,0.5);
        const auto& r=matches.front();
        std::cout<<std::fixed<<std::setprecision(6);
        std::cout<<r.object_name<<"\nNORAD      "<<r.norad_cat_id<<"\nKIND       PROPAGATED\nQUALITY    "<<quality<<"\n"
                 <<"EPOCH      "<<r.epoch<<"\nFRAME      ITRF (TEME/PEF/ITRF)\nORIGIN     EARTH CENTER\n"
                 <<"LAT        "<<tracked.geodetic.latitude_deg<<" deg\nLON        "<<tracked.geodetic.longitude_deg<<" deg\nALT        "<<tracked.geodetic.altitude_m/1000.0<<" km\n"
                 <<"AZ         "<<tracked.topocentric.azimuth_deg<<" deg\nEL         "<<tracked.topocentric.elevation_deg<<" deg\nRANGE      "<<tracked.topocentric.range/1000.0<<" km\n"
                 <<"DOPPLER    "<<tracked.doppler_hz<<" Hz\nLIGHT      "<<(tracked.illumination==orbit::Illumination::Sunlit?"SUNLIT":tracked.illumination==orbit::Illumination::Umbra?"UMBRA":"PENUMBRA")<<"\n";
        if (!passes.empty()) std::cout<<"AOS        "<<passes.front().aos_minutes-minutes<<" min\nMAX EL     "<<passes.front().max_elevation_deg<<" deg\nLOS        "<<passes.front().los_minutes-minutes<<" min\n";
        else std::cout<<"AOS        NONE NEXT 24H\n";
        std::cout<<"SOURCE     "<<r.source_id<<"\nHASH       "<<r.content_sha256<<"\nINGESTED   "<<r.ingested_at<<"\nAGE        "
                 <<(source_age_s >= 0.0 ? std::to_string(source_age_s)+" s" : "UNKNOWN")<<"\nMODEL      "<<r.model_version<<"\nEOP        "<<eop_quality<<"\nUNCERTAINTY UNKNOWN\n";
        return 0;
    }
    std::string q=args.size()>3?args[3]:"";
    std::size_t limit=25;
    if (args.size()>4) try { limit=static_cast<std::size_t>(std::stoul(args[4])); } catch (...) {}
    const auto list=astro::find_omm(parsed.records,q,limit);
    std::cout<<"SOURCE "<<id<<"\nRECORDS "<<parsed.records.size()<<"\n";
    std::cout<<std::left<<std::setw(28)<<"OBJECT"<<std::setw(9)<<"NORAD"<<std::setw(13)<<"INC DEG"<<std::setw(14)<<"MM REV/D"<<"EPOCH\n";
    for (const auto& r:list) std::cout<<std::left<<std::setw(28)<<r.object_name.substr(0,27)<<std::setw(9)<<r.norad_cat_id<<std::setw(13)<<std::fixed<<std::setprecision(5)<<r.inclination_deg<<std::setw(14)<<r.mean_motion_rev_day<<r.epoch<<"\n";
    return 0;
}

int App::seismic(const std::vector<std::string>& args) {
    std::string window="hour";
    int limit=20;
    if (args.size()>1) window=args[1];
    if (args.size()>2) try { limit=std::stoi(args[2]); } catch (...) {}
    const std::string id="usgs.earthquakes."+window;
    const auto c=catalog();
    if (!c) return 1;
    const auto s=c->find(id);
    if (!s) { std::cout<<"WINDOW MUST BE hour day OR week\n"; return 1; }
    const auto rr=data::SyncEngine(cache_root()).fetch(*s);
    if (!rr.ok) { std::cout<<"SEISMIC SYNC FAILED "<<rr.error<<"\n"; return 1; }
    data::CacheStore store(cache_root());
    const auto path=store.latest(id);
    if (!path) return 1;
    const auto feed=earth::load_usgs_geojson(*path);
    if (!feed.ok) { std::cout<<"SEISMIC PARSE FAILED "<<feed.error<<"\n"; return 1; }
    std::vector<earth::Earthquake> events=feed.feed.events;
    std::sort(events.begin(),events.end(),[](const auto& a,const auto& b){ return a.magnitude>b.magnitude; });
    std::cout<<"FEED "<<feed.feed.title<<"\nEVENTS "<<events.size()<<"\n";
    for (int i=0;i<std::min<int>(limit,static_cast<int>(events.size()));++i) {
        const auto& q=events[static_cast<std::size_t>(i)];
        std::cout<<"M"<<std::fixed<<std::setprecision(1)<<q.magnitude<<"  "<<std::setprecision(3)<<q.latitude_deg<<","<<q.longitude_deg<<"  DEPTH "<<q.depth_km<<" km  "<<q.place<<"\n";
    }
    return 0;
}

int App::spaceweather(const std::vector<std::string>&) {
    const auto c=catalog();
    if (!c) return 1;
    data::SyncEngine engine(cache_root());
    for (const auto& id:{std::string("noaa.swpc.kp"),std::string("noaa.swpc.solarwind.plasma"),std::string("noaa.swpc.solarwind.mag"),std::string("noaa.swpc.scales")}) {
        const auto s=c->find(id);
        if (s && s->syncable) engine.fetch(*s);
    }
    data::CacheStore store(cache_root());
    const auto kp_path=store.latest("noaa.swpc.kp");
    const auto sw_path=store.latest("noaa.swpc.solarwind.plasma");
    if (kp_path) {
        const auto text=json::read_text_file(*kp_path);
        if (text) {
            const auto samples=earth::parse_noaa_kp(*text);
            if (!samples.empty()) {
                const auto& v=samples.back();
                std::cout<<"KP TIME      "<<v.timestamp<<"\nKP           "<<v.kp<<"\nKP EST       "<<v.estimated_kp<<"\nSTATIONS     "<<v.station_count<<"\n";
            }
        }
    }
    if (sw_path) {
        const auto text=json::read_text_file(*sw_path);
        if (text) {
            const auto samples=earth::parse_noaa_solar_wind_plasma(*text);
            if (!samples.empty()) {
                const auto& v=samples.back();
                std::cout<<"SW TIME      "<<v.timestamp<<"\nSW SPEED     "<<v.speed_km_s<<" km/s\nSW DENSITY   "<<v.density_p_cm3<<" p/cm3\nSW TEMP      "<<v.temperature_k<<" K\n";
            }
        }
    }
    return 0;
}

int App::fireballs(const std::vector<std::string>& args) {
    int limit=20;
    if (args.size()>1) try { limit=std::stoi(args[1]); } catch (...) {}
    const auto c=catalog();
    if (!c) return 1;
    const auto s=c->find("jpl.fireballs");
    if (!s) return 1;
    const auto r=data::SyncEngine(cache_root()).fetch(*s);
    if (!r.ok) { std::cout<<"FIREBALL SYNC FAILED "<<r.error<<"\n"; return 1; }
    data::CacheStore store(cache_root());
    const auto path=store.latest(s->id);
    if (!path) return 1;
    const auto text=json::read_text_file(*path);
    if (!text) return 1;
    const auto feed=earth::parse_jpl_fireballs(*text);
    if (!feed) { std::cout<<"FIREBALL PARSE FAILED\n"; return 1; }
    std::cout<<"JPL FIREBALL API "<<feed->version<<"  EVENTS "<<feed->events.size()<<"\n";
    for (int i=0;i<std::min<int>(limit,static_cast<int>(feed->events.size()));++i) {
        const auto& f=feed->events[static_cast<std::size_t>(i)];
        std::cout<<f.date;
        if (f.latitude_deg && f.longitude_deg) std::cout<<"  "<<*f.latitude_deg<<","<<*f.longitude_deg;
        if (f.altitude_km) std::cout<<"  ALT "<<*f.altitude_km<<" km";
        if (f.velocity_km_s) std::cout<<"  V "<<*f.velocity_km_s<<" km/s";
        if (f.impact_energy_kt) std::cout<<"  E "<<*f.impact_energy_kt<<" kt";
        std::cout<<"\n";
    }
    return 0;
}

int App::station(const std::vector<std::string>& args) {
    if (args.size()>1 && args[1]=="interfaces") {
        const auto ifs=system::interface_addresses();
        for (const auto& i:ifs) std::cout<<std::left<<std::setw(18)<<i.name<<std::setw(6)<<i.family<<std::setw(8)<<(i.up?"UP":"DOWN")<<std::setw(6)<<(i.loopback?"LOOP":"")<<i.address<<"\n";
        return 0;
    }
    const auto s=system::station_snapshot();
    std::cout<<"HOST       "<<s.hostname<<"\nOS         "<<s.os<<"\nARCH       "<<s.architecture<<"\nCPU THREADS "<<s.logical_cpu_count<<"\nUPTIME     "<<s.uptime_seconds<<" s\nMEM TOTAL  "<<human_bytes(s.memory_total_bytes)<<"\nMEM AVAIL  "<<human_bytes(s.memory_available_bytes)<<"\nINTERFACES "<<system::interface_addresses().size()<<" addresses\n";
    return 0;
}

int App::universe(const std::vector<std::string>&) {
    const auto c=catalog();
    if (!c) return 1;
    std::map<std::string,int> domains;
    std::map<std::string,int> coverage;
    int public_sync=0;
    for (const auto& s:c->all()) {
        ++domains[s.domain];
        ++coverage[s.coverage];
        if (s.syncable && s.auth=="none") ++public_sync;
    }
    std::cout<<"NADIR DATA UNIVERSE 0.3\nSOURCES          "<<c->all().size()<<"\nPUBLIC SYNCABLE  "<<public_sync<<"\nDOMAINS          "<<domains.size()<<"\nCOVERAGE CLASSES "<<coverage.size()<<"\n\n";
    for (const auto& [d,n]:domains) std::cout<<std::left<<std::setw(12)<<d<<n<<"\n";
    return 0;
}

void App::banner() const {
    std::cout<<"\x1b[38;2;0;255;119m";
    std::cout<<"NADIR SYSTEMS CONSOLE\n";
    std::cout<<"NAVIGATIONAL & ASTRONOMICAL DATA INSTRUMENTATION RENDERER\n";
    std::cout<<"BUILD 000003 / C++23\n\n";
    std::cout<<"CORE   ONLINE\nWGS84  ONLINE\nRENDER ONLINE\nDATA   0.3 UNIVERSE\nORBIT  OMM INGEST\nEOP    IERS READY\nNET    STATION READY\n\nTYPE help FOR COMMANDS\n\x1b[0m";
}

void App::help() const {
    std::cout<<"earth [--yaw deg] [--pitch deg] [--lat deg] [--lon deg]\n";
    std::cout<<"earth live [--iss]  (cache: CelesTrak stations + IERS EOP)\n";
    std::cout<<"geo <lat_deg> <lon_deg> <alt_m>\n";
    std::cout<<"time\n";
    std::cout<<"eop [live]\n";
    std::cout<<"sources [domain]\n";
    std::cout<<"sources stats\n";
    std::cout<<"sources search <term>\n";
    std::cout<<"source <id>\n";
    std::cout<<"presets [name]\n";
    std::cout<<"sync <source-id|domain:name|preset:name>\n";
    std::cout<<"cache <source-id>\n";
    std::cout<<"ndr inspect <path>\nndr seek <path> <timestamp_ns>\n";
    std::cout<<"orbit list <source|group> [query] [limit]\n";
    std::cout<<"orbit live <source|group> [query] [limit]\n";
    std::cout<<"orbit inspect <source|group> <query> [lat lon alt_m frequency_hz]\n";
    std::cout<<"body info <name>\n";
    std::cout<<"body live <name>\n";
    std::cout<<"targets [query]\n";
    std::cout<<"target live <name|Horizons-ID>\n";
    std::cout<<"solar live\n";
    std::cout<<"seismic [hour|day|week] [limit]\n";
    std::cout<<"spaceweather\n";
    std::cout<<"fireballs [limit]\n";
    std::cout<<"craft <family> <generation>\n";
    std::cout<<"station [interfaces]\n";
    std::cout<<"universe\n";
    std::cout<<"tle <name> <line1> <line2>\n";
    std::cout<<"clear\nexit\n";
}

}

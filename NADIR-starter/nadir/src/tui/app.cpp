#include <nadir/tui/app.hpp>
#include <nadir/core/terminal.hpp>
#include <nadir/geo/wgs84.hpp>
#include <nadir/render/earth.hpp>
#include <nadir/render/framebuffer.hpp>
#include <nadir/satellite/tle.hpp>
#include <algorithm>
#include <chrono>
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

void App::banner() const {
    std::cout << "\x1b[38;2;0;255;119m";
    std::cout << "NADIR SYSTEMS CONSOLE\n";
    std::cout << "NAVIGATIONAL & ASTRONOMICAL\n";
    std::cout << "BUILD 000001 / C++23\n\n";
    std::cout << "CORE   ONLINE\n";
    std::cout << "WGS84  ONLINE\n";
    std::cout << "RENDER ONLINE\n";
    std::cout << "SGP4   PLANNED\n";
    std::cout << "NET    PLANNED\n\n";
    std::cout << "TYPE help FOR COMMANDS\n\x1b[0m";
}

void App::help() const {
    std::cout << "earth [--yaw deg] [--pitch deg] [--lat deg] [--lon deg]\n";
    std::cout << "geo <lat_deg> <lon_deg> <alt_m>\n";
    std::cout << "tle <name> <line1> <line2>\n";
    std::cout << "clear\n";
    std::cout << "exit\n";
}

}

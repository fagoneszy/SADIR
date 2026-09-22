#pragma once
#include <string>
#include <vector>

namespace nadir::tui {

class App {
public:
    int run(const std::vector<std::string>& args);

private:
    int shell();
    int command(const std::vector<std::string>& args);
    int earth(const std::vector<std::string>& args);
    int earth_live(const std::vector<std::string>& args);
    int geo(const std::vector<std::string>& args);
    int tle(const std::vector<std::string>& args);
    int sources(const std::vector<std::string>& args);
    int presets(const std::vector<std::string>& args);
    int source(const std::vector<std::string>& args);
    int sync(const std::vector<std::string>& args);
    int cache(const std::vector<std::string>& args);
    int gaia(const std::vector<std::string>& args);
    int ndr(const std::vector<std::string>& args);
    int body(const std::vector<std::string>& args);
    int targets(const std::vector<std::string>& args);
    int target(const std::vector<std::string>& args);
    int solar(const std::vector<std::string>& args);
    int craft(const std::vector<std::string>& args);
    int clock(const std::vector<std::string>& args);
    int eop(const std::vector<std::string>& args);
    int orbit(const std::vector<std::string>& args);
    int seismic(const std::vector<std::string>& args);
    int spaceweather(const std::vector<std::string>& args);
    int fireballs(const std::vector<std::string>& args);
    int station(const std::vector<std::string>& args);
    int universe(const std::vector<std::string>& args);
    void banner() const;
    void help() const;
};

}

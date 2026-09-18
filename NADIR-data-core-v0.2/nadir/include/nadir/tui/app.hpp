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
    int geo(const std::vector<std::string>& args);
    int tle(const std::vector<std::string>& args);
    int sources(const std::vector<std::string>& args);
    int sync(const std::vector<std::string>& args);
    int body(const std::vector<std::string>& args);
    int solar(const std::vector<std::string>& args);
    int craft(const std::vector<std::string>& args);
    int clock(const std::vector<std::string>& args);
    void banner() const;
    void help() const;
};

}

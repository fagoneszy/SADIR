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
    void banner() const;
    void help() const;
};

}

#pragma once
#include <optional>
#include <string>
#include <vector>

namespace nadir::spacecraft {

struct Component {
    std::string subsystem;
    std::string name;
    std::string value;
    std::string source;
};

struct Architecture {
    std::string family;
    std::string generation;
    std::vector<Component> components;
};

std::vector<Architecture> public_architectures();
std::optional<Architecture> find_architecture(const std::string& family,const std::string& generation);

}

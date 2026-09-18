#pragma once
#include <cstdint>
#include <string>

namespace nadir::network {

struct FlowKey {
    std::string source_ip;
    std::string destination_ip;
    std::uint16_t source_port{};
    std::uint16_t destination_port{};
    std::uint8_t protocol{};
};

struct FlowStats {
    FlowKey key;
    std::uint64_t packets{};
    std::uint64_t bytes{};
};

}

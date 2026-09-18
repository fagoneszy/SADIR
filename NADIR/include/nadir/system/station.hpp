#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace nadir::system {

struct StationSnapshot {
    std::string hostname;
    std::string os;
    std::string architecture;
    std::uint32_t logical_cpu_count{};
    std::uint64_t uptime_seconds{};
    std::uint64_t memory_total_bytes{};
    std::uint64_t memory_available_bytes{};
};

struct InterfaceAddress {
    std::string name;
    std::string family;
    std::string address;
    bool up{};
    bool loopback{};
};

StationSnapshot station_snapshot();
std::vector<InterfaceAddress> interface_addresses();

}

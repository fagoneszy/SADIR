#pragma once
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace nadir::core {

std::string sha256(std::span<const std::uint8_t> data);
std::string sha256(const std::string& data);
std::string sha256_file(const std::string& path);

}

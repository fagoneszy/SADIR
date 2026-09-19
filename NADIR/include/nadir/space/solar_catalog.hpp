#pragma once

#include <nadir/space/body.hpp>
#include <array>
#include <optional>

namespace nadir::space {

const BodyParams& solar_body(std::uint32_t index);
const BodyParams* solar_body_by_name(const std::string& name) noexcept;
const BodyParams* solar_body_by_horizons_id(int id) noexcept;
std::uint32_t solar_body_count() noexcept;

namespace detail {
constexpr double deg_to_rad(double deg) noexcept {
    constexpr double pi = 3.14159265358979323846;
    return deg * pi / 180.0;
}
}

}
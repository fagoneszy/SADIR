#pragma once

#include <vector>
#include <cstdint>
#include <array>

#include <nadir/math/vec3.hpp>

namespace nadir::render {

struct EarthMesh {
    std::vector<nadir::math::Vec3d> vertices;
    std::vector<std::array<std::uint32_t, 2>> lines;  // latitude/longitude lines
};

struct StarField {
    struct Star {
        nadir::math::Vec3d direction;
        float magnitude;
    };
    std::vector<Star> stars;
};

EarthMesh generate_earth_mesh(int latitude_steps, int longitude_steps);
StarField generate_star_field(int count);

}
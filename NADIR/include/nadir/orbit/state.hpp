#pragma once

#include <nadir/orbit/epoch.hpp>
#include <nadir/orbit/frame.hpp>
#include <nadir/orbit/vector.hpp>

namespace nadir::orbit {

struct State {
    Vec3d position_km;
    Vec3d velocity_km_s;
    Epoch epoch;
    Frame frame{Frame::Unknown};
};

}
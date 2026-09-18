#pragma once
#include <nadir/geo/wgs84.hpp>
#include <nadir/render/framebuffer.hpp>

namespace nadir::render {

struct EarthView {
    double yaw_deg{-28.0};
    double pitch_deg{18.0};
    double observer_lat_deg{-12.093};
    double observer_lon_deg{-45.786};
};

void draw_earth(Framebuffer& fb, const EarthView& view);

}

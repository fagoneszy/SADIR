#include <nadir/render/earth.hpp>
#include <nadir/render/line.hpp>
#include <cmath>
#include <optional>

namespace nadir::render {

static Vec3 sphere_point(double lat_deg, double lon_deg) {
    const double lat = lat_deg * deg_to_rad;
    const double lon = lon_deg * deg_to_rad;
    const double c = std::cos(lat);
    return {c * std::cos(lon), c * std::sin(lon), std::sin(lat)};
}

static std::optional<Vec2> project(const Vec3& p, const Mat3& r, int w, int h) {
    const Vec3 q = r * p;
    if (q.x < -0.02) return std::nullopt;
    const double s = std::min(w * 0.44, h * 0.44);
    return Vec2{w * 0.5 + q.y * s, h * 0.5 - q.z * s};
}

static void draw_curve(Framebuffer& fb, const Mat3& r, auto sample) {
    std::optional<Vec2> prev;
    for (int i = 0; i <= 180; ++i) {
        const auto point = sample(i / 180.0);
        const auto cur = project(point, r, fb.width(), fb.height());
        if (prev && cur) line(fb, static_cast<int>(prev->x), static_cast<int>(prev->y), static_cast<int>(cur->x), static_cast<int>(cur->y));
        prev = cur;
    }
}

void draw_earth(Framebuffer& fb, const EarthView& view) {
    const Mat3 r = Mat3::rotation_z(view.pitch_deg * deg_to_rad) * Mat3::rotation_y(view.yaw_deg * deg_to_rad);
    for (int lat = -75; lat <= 75; lat += 15) {
        draw_curve(fb, r, [lat](double t) { return sphere_point(static_cast<double>(lat), -180.0 + 360.0 * t); });
    }
    for (int lon = -180; lon < 180; lon += 15) {
        draw_curve(fb, r, [lon](double t) { return sphere_point(-90.0 + 180.0 * t, static_cast<double>(lon)); });
    }
    draw_curve(fb, r, [](double t) { return sphere_point(-90.0 + 180.0 * t, 0.0); });
    const auto marker = project(sphere_point(view.observer_lat_deg, view.observer_lon_deg), r, fb.width(), fb.height());
    if (marker) {
        const int x = static_cast<int>(marker->x);
        const int y = static_cast<int>(marker->y);
        for (int d = -2; d <= 2; ++d) {
            fb.set(x + d, y);
            fb.set(x, y + d);
        }
    }
}

}

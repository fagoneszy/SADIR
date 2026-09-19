#include <nadir/render/camera.hpp>
#include <nadir/math/quat.hpp>
#include <cmath>
#include <algorithm>

namespace nadir::render {

namespace {
constexpr double pi = 3.141592653589793238462643383279502884;
}

void Camera::yaw(double radians) noexcept {
    const nadir::math::Quatd q = nadir::math::Quatd::from_axis_angle(
        nadir::math::Vec3d{0.0, 1.0, 0.0}, radians);
    orientation = orientation * q;
}

void Camera::pitch(double radians) noexcept {
    const nadir::math::Quatd q = nadir::math::Quatd::from_axis_angle(
        nadir::math::Vec3d{1.0, 0.0, 0.0}, radians);
    orientation = orientation * q;
}

void Camera::roll(double radians) noexcept {
    const nadir::math::Quatd q = nadir::math::Quatd::from_axis_angle(
        nadir::math::Vec3d{0.0, 0.0, 1.0}, radians);
    orientation = orientation * q;
}

void Camera::zoom(double delta) noexcept {
    distance = std::max(0.1, distance * std::exp(-delta * 0.1));
}

nadir::math::Vec3d Camera::position() const noexcept {
    const nadir::math::Vec3d forward = orientation.rotate({0.0, 0.0, -1.0});
    return target - forward * distance;
}

nadir::math::Vec3d Camera::world_to_view(const nadir::math::Vec3d& world) const noexcept {
    const nadir::math::Vec3d local = world - position();
    return orientation.conjugate().rotate(local);
}

}
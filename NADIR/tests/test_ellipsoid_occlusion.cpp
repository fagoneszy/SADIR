#include <nadir/render/ellipsoid_occlusion.hpp>

#include <limits>

int main() {
    using nadir::math::Vec3d;
    using namespace nadir::render;

    const Ellipsoid unit{1.0, 1.0};
    if (occluded_by_ellipsoid({0.0, 0.0, 3.0}, {0.0, 0.0, 2.0}, unit)) return 1;
    if (!occluded_by_ellipsoid({0.0, 0.0, 3.0}, {0.0, 0.0, -3.0}, unit)) return 2;
    if (occluded_by_ellipsoid({0.0, 0.0, 3.0}, {3.0, 0.0, 0.0}, unit)) return 3;
    if (!segment_intersects_ellipsoid({0.0, 0.0, 3.0}, {1.0, 0.0, -3.0}, unit)) return 4;
    if (segment_intersects_ellipsoid({0.0, 0.0, 3.0}, {0.0, 0.0, 3.0}, unit)) return 5;
    if (segment_intersects_ellipsoid({std::numeric_limits<double>::quiet_NaN(), 0.0, 3.0},
                                     {0.0, 0.0, -3.0}, unit)) return 6;

    const auto wgs84 = wgs84_ellipsoid();
    if (!occluded_by_ellipsoid({0.0, 0.0, 7000000.0}, {0.0, 0.0, -7000000.0}, wgs84)) return 7;
    if (occluded_by_ellipsoid({0.0, 0.0, 7000000.0}, {0.0, 0.0, 6800000.0}, wgs84)) return 8;
    if (!occluded_by_ellipsoid({7000000.0, 0.0, 0.0}, {-7000000.0, 0.0, 0.0}, wgs84)) return 9;
    return 0;
}

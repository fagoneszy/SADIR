#include <nadir/orbit/eclipse.hpp>
int main() {
    using namespace nadir::orbit;
    constexpr Vec3d sun{149597870.7, 0.0, 0.0};
    if (classify_earth_eclipse({7000.0, 0.0, 0.0}, sun) != Illumination::Sunlit) return 1;
    if (classify_earth_eclipse({-7000.0, 0.0, 0.0}, sun) != Illumination::Umbra) return 2;
    if (classify_earth_eclipse({-7000.0, 6400.0, 0.0}, sun) == Illumination::Invalid) return 3;
    return classify_earth_eclipse({}, sun, 0.0) == Illumination::Invalid ? 0 : 4;
}

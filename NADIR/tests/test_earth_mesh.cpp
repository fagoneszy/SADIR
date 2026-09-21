#include <nadir/render/earth_mesh.hpp>

#include <cmath>

int main() {
    const auto mesh = nadir::render::generate_earth_mesh(12, 24);
    if (mesh.vertices.empty() || mesh.lines.empty()) return 1;
    bool saw_equator = false;
    bool saw_pole = false;
    for (const auto& point : mesh.vertices) {
        if (!std::isfinite(point.x) || !std::isfinite(point.y) || !std::isfinite(point.z)) return 2;
        if (std::abs(point.z) < 1.0e-6 && std::abs(point.x) > 6370000.0) saw_equator = true;
        if (std::abs(point.z) > 6350000.0) saw_pole = true;
    }
    if (!saw_equator || !saw_pole) return 3;
    return 0;
}

#include <nadir/orbit/conjunction.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <unordered_map>

namespace nadir::orbit {
namespace {

struct Cell {
    std::int64_t x{};
    std::int64_t y{};
    std::int64_t z{};
    [[nodiscard]] bool operator==(const Cell&) const noexcept = default;
};

struct CellHash {
    [[nodiscard]] std::size_t operator()(const Cell& cell) const noexcept {
        const auto mix = [](std::uint64_t value) {
            value ^= value >> 30;
            value *= 0xbf58476d1ce4e5b9ULL;
            value ^= value >> 27;
            value *= 0x94d049bb133111ebULL;
            return value ^ (value >> 31);
        };
        return static_cast<std::size_t>(mix(static_cast<std::uint64_t>(cell.x)) ^
                                        mix(static_cast<std::uint64_t>(cell.y)) ^
                                        mix(static_cast<std::uint64_t>(cell.z)));
    }
};

[[nodiscard]] bool finite(const math::Vec3d& value) noexcept {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}

[[nodiscard]] bool cell_for(const math::Vec3d& position, double cell_size, Cell& cell) noexcept {
    constexpr auto limit = static_cast<long double>(std::numeric_limits<std::int64_t>::max() - 1);
    const auto coordinate = [cell_size](double value, std::int64_t& output) {
        const long double scaled = std::floor(static_cast<long double>(value) / cell_size);
        if (!std::isfinite(static_cast<double>(scaled)) || scaled < -limit || scaled > limit) return false;
        output = static_cast<std::int64_t>(scaled);
        return true;
    };
    return coordinate(position.x, cell.x) && coordinate(position.y, cell.y) && coordinate(position.z, cell.z);
}

} // namespace

std::vector<Conjunction> screen_conjunctions(const std::vector<ScreeningObject>& objects, double horizon, double threshold) {
    std::vector<Conjunction> result;
    if (!std::isfinite(horizon) || !std::isfinite(threshold) || horizon < 0.0 || threshold < 0.0) return result;
    double maximum_speed{};
    for (const auto& object : objects) {
        if (finite(object.position_m) && finite(object.velocity_m_s))
            maximum_speed = std::max(maximum_speed, object.velocity_m_s.norm());
    }
    // No relative speed can exceed twice the greatest object speed. Cells of this
    // width therefore contain every pair that can reach the screening threshold.
    const double reach = threshold + 2.0 * maximum_speed * horizon;
    const auto consider = [&](std::size_t i, std::size_t j) {
        const auto& a=objects[i]; const auto& b=objects[j];
        if (a.id == 0 || b.id == 0 || a.id == b.id) return;
        const auto dr=b.position_m-a.position_m, dv=b.velocity_m_s-a.velocity_m_s;
        const double speed=dv.norm(), distance=dr.norm();
        // Conservative broadphase: even a head-on relative velocity cannot close more than v*T.
        if (distance > threshold + speed*horizon) return;
        const double vv=dv.dot(dv);
        const double t=vv>0.0?std::clamp(-dr.dot(dv)/vv,0.0,horizon):0.0;
        const double miss=(dr+dv*t).norm();
        if (miss <= threshold) result.push_back({std::min(a.id,b.id),std::max(a.id,b.id),t,miss,speed});
    };
    if (!std::isfinite(reach)) {
        for (std::size_t i = 0; i < objects.size(); ++i)
            for (std::size_t j = i + 1; j < objects.size(); ++j) consider(i, j);
        std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
            if (a.tca_seconds != b.tca_seconds) return a.tca_seconds < b.tca_seconds;
            if (a.miss_distance_m != b.miss_distance_m) return a.miss_distance_m < b.miss_distance_m;
            return a.first_id != b.first_id ? a.first_id < b.first_id : a.second_id < b.second_id;
        });
        return result;
    }
    const double cell_size = reach > 0.0 ? reach : 1.0;
    std::unordered_map<Cell, std::vector<std::size_t>, CellHash> grid;
    std::vector<std::size_t> ungridded;
    std::vector<bool> is_ungridded(objects.size());
    for (std::size_t index = 0; index < objects.size(); ++index) {
        Cell cell;
        if (finite(objects[index].position_m) && finite(objects[index].velocity_m_s) &&
            cell_for(objects[index].position_m, cell_size, cell)) grid[cell].push_back(index);
        else if (finite(objects[index].position_m) && finite(objects[index].velocity_m_s)) {
            ungridded.push_back(index);
            is_ungridded[index] = true;
        }
    }
    for (const auto& [cell, indices] : grid) {
        for (std::size_t i = 0; i < indices.size(); ++i)
            for (std::size_t j = i + 1; j < indices.size(); ++j) consider(indices[i], indices[j]);
        for (std::int64_t dx = -1; dx <= 1; ++dx) for (std::int64_t dy = -1; dy <= 1; ++dy)
            for (std::int64_t dz = -1; dz <= 1; ++dz) {
                if (dx == 0 && dy == 0 && dz == 0) continue;
                const Cell neighbor{cell.x + dx, cell.y + dy, cell.z + dz};
                if (neighbor.x < cell.x || (neighbor.x == cell.x &&
                    (neighbor.y < cell.y || (neighbor.y == cell.y && neighbor.z <= cell.z)))) continue;
                const auto found = grid.find(neighbor);
                if (found == grid.end()) continue;
                for (const auto i : indices) for (const auto j : found->second) consider(i, j);
            }
    }
    // Coordinates outside the integer grid are rare, but must still retain the
    // exact-screening contract instead of silently losing a candidate.
    for (const auto i : ungridded)
        for (std::size_t j = 0; j < objects.size(); ++j)
            if (j != i && (!is_ungridded[j] || i < j)) consider(i, j);
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        if (a.tca_seconds != b.tca_seconds) return a.tca_seconds < b.tca_seconds;
        if (a.miss_distance_m != b.miss_distance_m) return a.miss_distance_m < b.miss_distance_m;
        return a.first_id != b.first_id ? a.first_id < b.first_id : a.second_id < b.second_id;
    });
    return result;
}
} // namespace nadir::orbit

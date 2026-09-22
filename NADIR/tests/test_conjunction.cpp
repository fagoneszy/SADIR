#include <nadir/orbit/conjunction.hpp>

#include <cmath>
#include <limits>
#include <vector>
int main() {
    using nadir::orbit::ScreeningObject;
    const auto hits=nadir::orbit::screen_conjunctions({{1,{0,0,0},{1,0,0}},{2,{100,10,0},{-1,0,0}},{3,{100000,0,0},{}}},100.0,20.0);
    if (hits.size()!=1 || hits[0].first_id!=1 || hits[0].second_id!=2) return 1;
    if (std::abs(hits[0].tca_seconds-50.0)>1e-12 || std::abs(hits[0].miss_distance_m-10.0)>1e-12) return 2;
    std::vector<ScreeningObject> sparse;
    sparse.reserve(4098);
    for (std::uint64_t id = 1; id <= 4096; ++id)
        sparse.push_back({id, {static_cast<double>(id) * 1'000'000.0, 0.0, 0.0}, {}});
    sparse.push_back({5001, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}});
    sparse.push_back({5002, {100.0, 10.0, 0.0}, {-1.0, 0.0, 0.0}});
    const auto sparse_hits = nadir::orbit::screen_conjunctions(sparse, 100.0, 20.0);
    if (sparse_hits.size() != 1 || sparse_hits[0].first_id != 5001 || sparse_hits[0].second_id != 5002) return 3;
    const double huge = std::numeric_limits<double>::max();
    if (nadir::orbit::screen_conjunctions({{7001, {huge, 0.0, 0.0}, {}}, {7002, {huge, 0.0, 0.0}, {}}}, 0.0, 0.0).size() != 1) return 4;
    nadir::state::Covariance6 covariance{};
    covariance.values[0] = covariance.values[7] = covariance.values[14] = 10'000.0;
    covariance.values[21] = covariance.values[28] = covariance.values[35] = 1.0;
    const auto probability = nadir::orbit::collision_probability_encounter_plane({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, covariance, covariance, 10.0);
    const auto expected = 1.0 - std::exp(-100.0 / 40'000.0);
    if (!probability || std::abs(*probability - expected) > 1.0e-6) return 5;
    const auto rotated_probability = nadir::orbit::collision_probability_encounter_plane({0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, covariance, covariance, 10.0);
    if (!rotated_probability || std::abs(*rotated_probability - *probability) > 1.0e-9) return 6;
    if (nadir::orbit::collision_probability_encounter_plane({}, {}, covariance, covariance, 10.0)) return 7;
    return nadir::orbit::screen_conjunctions({},-1.0,1.0).empty()?0:8;
}

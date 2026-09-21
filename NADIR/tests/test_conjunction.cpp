#include <nadir/orbit/conjunction.hpp>

#include <cmath>
int main() {
    using nadir::orbit::ScreeningObject;
    const auto hits=nadir::orbit::screen_conjunctions({{1,{0,0,0},{1,0,0}},{2,{100,10,0},{-1,0,0}},{3,{100000,0,0},{}}},100.0,20.0);
    if (hits.size()!=1 || hits[0].first_id!=1 || hits[0].second_id!=2) return 1;
    if (std::abs(hits[0].tca_seconds-50.0)>1e-12 || std::abs(hits[0].miss_distance_m-10.0)>1e-12) return 2;
    return nadir::orbit::screen_conjunctions({},-1.0,1.0).empty()?0:3;
}

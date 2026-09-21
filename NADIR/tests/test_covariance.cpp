#include <nadir/state/covariance.hpp>

#include <cmath>

int main() {
    nadir::state::Covariance6 covariance{};
    covariance.values[0] = 4.0; covariance.values[7] = 9.0; covariance.values[14] = 16.0;
    covariance.values[21] = 1.0; covariance.values[28] = 1.0; covariance.values[35] = 1.0;
    if (!nadir::state::valid_covariance(covariance)) return 1;
    const auto sigma=nadir::state::position_sigma_m({true,covariance});
    if (!sigma || std::abs(*sigma-4.0)>1e-12) return 2;
    const auto rotated=nadir::state::rotate_covariance(covariance,{0,-1,0,1,0,0,0,0,1});
    if (std::abs(rotated.values[0]-9.0)>1e-12 || std::abs(rotated.values[7]-4.0)>1e-12) return 3;
    covariance.values[1]=1.0; return nadir::state::valid_covariance(covariance)?4:0;
}

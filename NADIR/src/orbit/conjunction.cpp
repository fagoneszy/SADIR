#include <nadir/orbit/conjunction.hpp>

#include <algorithm>
#include <cmath>

namespace nadir::orbit {
std::vector<Conjunction> screen_conjunctions(const std::vector<ScreeningObject>& objects, double horizon, double threshold) {
    std::vector<Conjunction> result;
    if (!std::isfinite(horizon) || !std::isfinite(threshold) || horizon < 0.0 || threshold < 0.0) return result;
    for (std::size_t i = 0; i < objects.size(); ++i) for (std::size_t j = i + 1; j < objects.size(); ++j) {
        const auto& a=objects[i]; const auto& b=objects[j];
        if (a.id == 0 || b.id == 0 || a.id == b.id) continue;
        const auto dr=b.position_m-a.position_m, dv=b.velocity_m_s-a.velocity_m_s;
        const double speed=dv.norm(), distance=dr.norm();
        // Conservative broadphase: even a head-on relative velocity cannot close more than v*T.
        if (distance > threshold + speed*horizon) continue;
        const double vv=dv.dot(dv);
        const double t=vv>0.0?std::clamp(-dr.dot(dv)/vv,0.0,horizon):0.0;
        const double miss=(dr+dv*t).norm();
        if (miss <= threshold) result.push_back({std::min(a.id,b.id),std::max(a.id,b.id),t,miss,speed});
    }
    std::sort(result.begin(),result.end(),[](const auto& a,const auto& b){ return a.tca_seconds!=b.tca_seconds?a.tca_seconds<b.tca_seconds:a.miss_distance_m<b.miss_distance_m; });
    return result;
}
} // namespace nadir::orbit

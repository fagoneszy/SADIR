#include <nadir/space/ephemeris_provider.hpp>
#include <cmath>

int main() {
    nadir::space::BodyState earth{}; earth.utc_ns=42; earth.heliocentric_position_km={1.0,2.0,3.0};
    nadir::space::BodyState moon{}; moon.utc_ns=42; moon.heliocentric_position_km={385'401.0,2.0,3.0};
    const auto body=nadir::space::geocentric_third_body(moon,earth,4.9048695e12);
    if (!body || std::abs(body->position_m.x - 385'400'000.0) > 1.0 || std::abs(body->position_m.y) > 1.0e-9) return 1;
    moon.utc_ns=43;
    return nadir::space::geocentric_third_body(moon,earth,4.9048695e12) ? 2 : 0;
}

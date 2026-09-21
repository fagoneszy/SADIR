#include <nadir/geo/eop.hpp>

int main() {
    nadir::geo::EopTable table;
    table.records = {{60000.0, 0.1, 0.2, -0.1, 0.0, 0.0, 0.0, false},
                     {60001.0, 0.3, 0.4, -0.3, 0.0, 0.0, 0.0, true}};
    const auto observed = nadir::geo::resolve_eop(table, 60000.0);
    if (!observed || observed.availability != nadir::geo::EopAvailability::Observed) return 1;
    const auto predicted = nadir::geo::resolve_eop(table, 60000.5);
    if (!predicted || predicted.availability != nadir::geo::EopAvailability::Predicted || predicted.record.dut1_s != -0.2) return 2;
    return nadir::geo::resolve_eop(table, 59999.0) ? 3 : 0;
}

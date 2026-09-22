#include <nadir/astro/sp3.hpp>

#include <cmath>
#include <sstream>

namespace nadir::astro {
namespace {
constexpr std::size_t maximum_message_bytes = 16U * 1024U * 1024U;
constexpr std::size_t maximum_lines = 1'000'000U;
std::string trim(std::string value) { const auto first=value.find_first_not_of(" \t\r"); const auto last=value.find_last_not_of(" \t\r"); return first==std::string::npos?std::string{}:value.substr(first,last-first+1); }
bool number(std::istringstream& stream, double& value) { return static_cast<bool>(stream>>value) && std::isfinite(value); }
} // namespace

Sp3ParseResult parse_sp3(const std::string& text) {
    if (text.size() > maximum_message_bytes) return {false, {}, "SP3 message exceeds size limit"};
    Sp3Record record; bool header_seen{}; std::string epoch; std::size_t line_count{};
    std::istringstream input(text);
    for (std::string line; std::getline(input,line); ) {
        if (++line_count > maximum_lines) return {false, {}, "SP3 message has too many lines"};
        if (line.starts_with("#")) {
            if (header_seen || line.size()<3 || line[1]==' ') return {false, {}, "invalid SP3 header"};
            record.version=line[1]; header_seen=true;
        } else if (line.starts_with("%c")) {
            std::istringstream fields(line.substr(2)); std::string token;
            while (fields>>token) if (token=="GPS" || token=="UTC" || token=="GAL" || token=="GLO" || token=="TAI") { record.time_system=token; break; }
        } else if (line.starts_with("*")) {
            epoch=trim(line.substr(1)); if (epoch.empty()) return {false, {}, "empty SP3 epoch"};
        } else if (line.starts_with("P")) {
            if (!header_seen || epoch.empty() || line.size()<4) return {false, {}, "SP3 position without header or epoch"};
            const auto satellite_id=trim(line.substr(1,3)); std::istringstream values(line.substr(4)); double x{},y{},z{},clock{};
            if (satellite_id.empty() || !number(values,x) || !number(values,y) || !number(values,z) || !number(values,clock) || std::abs(x)>=999'999.0 || std::abs(y)>=999'999.0 || std::abs(z)>=999'999.0) return {false, {}, "invalid SP3 position"};
            Sp3Sample sample{epoch,satellite_id,{x*1000.0,y*1000.0,z*1000.0},{}};
            if (std::abs(clock)<999'999.0) sample.clock_offset_s=clock*1.0e-6;
            record.samples.push_back(std::move(sample));
        }
    }
    if (!header_seen || record.samples.empty()) return {false, {}, "missing SP3 header or positions"};
    return {true,std::move(record),{}};
}
} // namespace nadir::astro

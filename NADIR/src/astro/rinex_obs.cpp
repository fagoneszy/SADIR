#include <nadir/astro/rinex_obs.hpp>

#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>

namespace nadir::astro {
namespace {
constexpr std::size_t max_bytes = 16U * 1024U * 1024U;
constexpr std::size_t max_lines = 1'000'000U;

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r");
    const auto last = value.find_last_not_of(" \t\r");
    return first == std::string::npos ? std::string{} : value.substr(first, last - first + 1);
}

std::optional<double> measurement(std::string text) {
    text = trim(std::move(text));
    if (text.empty()) return {};
    for (auto& character : text) if (character == 'D' || character == 'd') character = 'E';
    try {
        std::size_t used{};
        const auto value = std::stod(text, &used);
        return used == text.size() && std::isfinite(value) ? std::optional<double>{value} : std::nullopt;
    } catch (...) { return std::nullopt; }
}

std::string epoch_text(int year, int month, int day, int hour, int minute, double second) {
    std::ostringstream output;
    output << std::setfill('0') << std::setw(4) << year << '-' << std::setw(2) << month << '-'
           << std::setw(2) << day << 'T' << std::setw(2) << hour << ':' << std::setw(2) << minute
           << ':' << std::fixed << std::setprecision(3) << std::setw(6) << second;
    return output.str();
}
} // namespace

RinexObservationParseResult parse_rinex_observation_epochs(const std::string& text) {
    if (text.size() > max_bytes) return {false, {}, "RINEX observation exceeds size limit"};
    std::istringstream input(text);
    std::string line;
    bool header_seen{}, header_end{};
    std::size_t line_count{};
    std::map<char, std::vector<std::string>> types;
    std::map<char, std::size_t> expected_types;
    char continuation_system{};
    std::vector<RinexObservationEpoch> epochs;
    const auto next = [&]() { return static_cast<bool>(std::getline(input, line)) && ++line_count <= max_lines; };

    while (next()) {
        if (!header_seen) {
            if (line.find("RINEX VERSION / TYPE") != std::string::npos) header_seen = true;
            continue;
        }
        if (!header_end) {
            const auto type_label = line.find("SYS / # / OBS TYPES");
            if (type_label != std::string::npos) {
                const auto body = line.substr(0, type_label);
                const auto first = body.find_first_not_of(' ');
                char system = continuation_system;
                std::string codes = body;
                if (first != std::string::npos && first == 0) {
                    std::istringstream fields(body);
                    int expected{};
                    if (!(fields >> system >> expected) || expected < 0) return {false, {}, "invalid RINEX observation types"};
                    continuation_system = system;
                    expected_types[system] = static_cast<std::size_t>(expected);
                    std::getline(fields, codes);
                } else if (!system || !expected_types.contains(system)) return {false, {}, "orphan RINEX observation type continuation"};
                std::istringstream fields(codes);
                for (std::string code; fields >> code; ) {
                    if (code.size() != 3 || types[system].size() >= expected_types[system]) return {false, {}, "invalid RINEX observation type"};
                    types[system].push_back(std::move(code));
                }
            }
            if (line.find("END OF HEADER") != std::string::npos) {
                header_end = true;
                for (const auto& [system, expected] : expected_types)
                    if (types[system].size() != expected) return {false, {}, "incomplete RINEX observation types"};
            }
            continue;
        }
        if (line.empty() || line[0] != '>') continue;
        std::istringstream fields(line.substr(1));
        int year{}, month{}, day{}, hour{}, minute{}, flag{}, count{};
        double second{};
        if (!(fields >> year >> month >> day >> hour >> minute >> second >> flag >> count) || !std::isfinite(second) ||
            year < 1900 || month < 1 || month > 12 || day < 1 || day > 31 || hour < 0 || hour > 23 ||
            minute < 0 || minute > 59 || second < 0.0 || second >= 61.0 || flag < 0 || flag > 6 || count < 0)
            return {false, {}, "invalid RINEX observation epoch"};
        epochs.push_back({epoch_text(year, month, day, hour, minute, second), flag, count, {}});
        if (flag > 1) continue;
        for (int satellite = 0; satellite < count; ++satellite) {
            if (!next()) return {false, {}, "truncated RINEX observation epoch"};
            if (line.size() < 3) return {false, {}, "invalid RINEX satellite observation"};
            const auto id = trim(line.substr(0, 3));
            if (id.size() != 3) return {false, {}, "invalid RINEX satellite id"};
            const auto found = types.find(id[0]);
            if (found == types.end()) continue;
            std::string payload = line.substr(3);
            const auto continuation_lines = (found->second.size() + 4U) / 5U;
            for (std::size_t continuation = 1; continuation < continuation_lines; ++continuation) {
                if (!next() || line.size() < 3 || !trim(line.substr(0, 3)).empty()) return {false, {}, "truncated RINEX observation continuation"};
                payload += line.substr(3);
            }
            for (std::size_t index{}; index < found->second.size(); ++index) {
                const auto offset = 16U * index;
                const auto field = offset < payload.size() ? payload.substr(offset, std::min<std::size_t>(16, payload.size() - offset)) : std::string{};
                const auto value = measurement(field.substr(0, std::min<std::size_t>(14, field.size())));
                if (!value && !trim(field).empty()) return {false, {}, "invalid RINEX observation value"};
                epochs.back().observations.push_back({id, found->second[index], value});
            }
        }
    }
    if (line_count > max_lines) return {false, {}, "RINEX observation has too many lines"};
    if (!header_seen || !header_end || epochs.empty()) return {false, {}, "missing RINEX observation header or epochs"};
    return {true, std::move(epochs), {}};
}
} // namespace nadir::astro

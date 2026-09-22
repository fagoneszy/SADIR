#include <nadir/astro/clk.hpp>
#include <cmath>
#include <iomanip>
#include <sstream>
namespace nadir::astro {
namespace { constexpr std::size_t max_bytes=16U*1024U*1024U, max_lines=1'000'000U; }
ClkParseResult parse_rinex_clk(const std::string& text) {
    if (text.size()>max_bytes) return {false,{},"CLK message exceeds size limit"};
    ClkRecord record; bool header{}, end{}; std::size_t lines{}; std::istringstream input(text);
    for (std::string line; std::getline(input,line); ) {
        if (++lines>max_lines) return {false,{},"CLK message has too many lines"};
        if (!header) {
            if (line.find("RINEX VERSION / TYPE")!=std::string::npos) { std::istringstream version(line); if (!(version>>record.version) || !std::isfinite(record.version)) return {false,{},"invalid CLK version"}; header=true; }
            continue;
        }
        if (!end) { if (line.find("END OF HEADER")!=std::string::npos) end=true; continue; }
        std::istringstream fields(line); std::string kind,id; int year{},month{},day{},hour{},minute{},count{}; double second{},bias{},sigma{};
        if (!(fields>>kind)) continue;
        if (kind!="AS") continue;
        if (!(fields>>id>>year>>month>>day>>hour>>minute>>second>>count>>bias) || id.empty() || count<1 || !std::isfinite(second) || !std::isfinite(bias)) return {false,{},"invalid CLK sample"};
        ClkSample sample; std::ostringstream epoch; epoch<<std::setfill('0')<<std::setw(4)<<year<<'-'<<std::setw(2)<<month<<'-'<<std::setw(2)<<day<<'T'<<std::setw(2)<<hour<<':'<<std::setw(2)<<minute<<':'<<std::fixed<<std::setprecision(9)<<std::setw(12)<<second; sample.epoch=epoch.str(); sample.satellite_id=id; sample.clock_bias_s=bias;
        if (fields>>sigma) { if (!std::isfinite(sigma) || sigma<0.0) return {false,{},"invalid CLK sigma"}; sample.clock_bias_sigma_s=sigma; }
        record.samples.push_back(std::move(sample));
    }
    if (!header || !end || record.samples.empty()) return {false,{},"missing CLK header or samples"};
    return {true,std::move(record),{}};
}
} // namespace nadir::astro

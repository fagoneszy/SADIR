#include <nadir/core/time.hpp>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nadir::core {

TimeState now_utc() {
    const auto tp=std::chrono::system_clock::now();
    const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
    const double jd=2440587.5+static_cast<double>(ns)/86400.0e9;
    return {ns,jd,jd-2400000.5};
}

static std::tm utc_tm(std::time_t t) {
    std::tm tm{};
#ifdef _WIN32
    gmtime_s(&tm,&t);
#else
    gmtime_r(&t,&tm);
#endif
    return tm;
}

std::string iso8601_utc(std::int64_t unix_ns) {
    const auto sec=unix_ns/1000000000LL;
    const auto rem=unix_ns%1000000000LL;
    const auto tm=utc_tm(static_cast<std::time_t>(sec));
    std::ostringstream out;
    out<<std::put_time(&tm,"%Y-%m-%dT%H:%M:%S")<<'.'<<std::setw(9)<<std::setfill('0')<<rem<<'Z';
    return out.str();
}

std::string compact_utc(std::int64_t unix_ns) {
    const auto sec=unix_ns/1000000000LL;
    const auto tm=utc_tm(static_cast<std::time_t>(sec));
    std::ostringstream out;
    out<<std::put_time(&tm,"%Y%m%dT%H%M%SZ");
    return out.str();
}

}

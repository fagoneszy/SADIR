#include <nadir/core/time.hpp>
#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nadir::core {

struct LeapEntry {
    int y;
    unsigned m;
    unsigned d;
    int offset;
};

static constexpr std::array<LeapEntry,28> leap_table{{
    {1972,1,1,10},{1972,7,1,11},{1973,1,1,12},{1974,1,1,13},{1975,1,1,14},{1976,1,1,15},{1977,1,1,16},
    {1978,1,1,17},{1979,1,1,18},{1980,1,1,19},{1981,7,1,20},{1982,7,1,21},{1983,7,1,22},{1985,7,1,23},
    {1988,1,1,24},{1990,1,1,25},{1991,1,1,26},{1992,7,1,27},{1993,7,1,28},{1994,7,1,29},{1996,1,1,30},
    {1997,7,1,31},{1999,1,1,32},{2006,1,1,33},{2009,1,1,34},{2012,7,1,35},{2015,7,1,36},{2017,1,1,37}
}};

static std::int64_t unix_seconds(int y,unsigned m,unsigned d) {
    using namespace std::chrono;
    const auto tp=sys_days{year{y}/month{m}/day{d}};
    return duration_cast<seconds>(tp.time_since_epoch()).count();
}

int tai_minus_utc(std::int64_t unix_ns) {
    const auto sec=unix_ns/1000000000LL;
    int offset=10;
    for (const auto& e:leap_table) {
        if (sec<unix_seconds(e.y,e.m,e.d)) break;
        offset=e.offset;
    }
    return offset;
}

TimeState time_from_unix_ns(std::int64_t unix_ns,double dut1_s) {
    const double jd_utc=2440587.5+static_cast<double>(unix_ns)/86400.0e9;
    const int leap=tai_minus_utc(unix_ns);
    const double jd_tai=jd_utc+static_cast<double>(leap)/86400.0;
    const double jd_tt=jd_tai+32.184/86400.0;
    const double jd_ut1=jd_utc+dut1_s/86400.0;
    return {unix_ns,jd_utc,jd_utc-2400000.5,leap,jd_tai,jd_tt,dut1_s,jd_ut1};
}

TimeState now_utc(double dut1_s) {
    const auto tp=std::chrono::system_clock::now();
    const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
    return time_from_unix_ns(ns,dut1_s);
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
    auto sec=unix_ns/1000000000LL;
    auto rem=unix_ns%1000000000LL;
    if (rem<0) { rem+=1000000000LL; --sec; }
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

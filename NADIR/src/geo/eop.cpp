#include <nadir/geo/eop.hpp>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <unordered_map>

namespace nadir::geo {

static std::string trim(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    return s;
}

static std::string key(std::string s) {
    std::string out;
    for (unsigned char c:s) if (std::isalnum(c)) out.push_back(static_cast<char>(std::tolower(c)));
    return out;
}

static std::vector<std::string> split(const std::string& line,char d) {
    std::vector<std::string> out;
    std::string cur;
    bool quote=false;
    for (std::size_t i=0;i<line.size();++i) {
        const char c=line[i];
        if (c=='\"') {
            if (quote && i+1<line.size() && line[i+1]=='\"') { cur.push_back('\"'); ++i; }
            else quote=!quote;
        } else if (c==d && !quote) {
            out.push_back(trim(cur));
            cur.clear();
        } else cur.push_back(c);
    }
    out.push_back(trim(cur));
    return out;
}

static std::optional<double> to_double(const std::string& s) {
    if (s.empty() || s=="---" || s=="null" || s=="NULL") return std::nullopt;
    try {
        std::size_t n=0;
        const double v=std::stod(s,&n);
        if (n==0 || !std::isfinite(v)) return std::nullopt;
        return v;
    } catch (...) { return std::nullopt; }
}

static int find_col(const std::vector<std::string>& h,const std::vector<std::string>& names) {
    for (std::size_t i=0;i<h.size();++i) {
        const auto k=key(h[i]);
        for (const auto& n:names) if (k==key(n)) return static_cast<int>(i);
    }
    for (std::size_t i=0;i<h.size();++i) {
        const auto k=key(h[i]);
        for (const auto& n:names) if (k.find(key(n))!=std::string::npos) return static_cast<int>(i);
    }
    return -1;
}

static std::optional<double> at(const std::vector<std::string>& row,int idx) {
    if (idx<0 || static_cast<std::size_t>(idx)>=row.size()) return std::nullopt;
    return to_double(row[static_cast<std::size_t>(idx)]);
}

std::optional<EopTable> load_iers_csv(const std::string& path) {
    std::ifstream in(path);
    if (!in) return std::nullopt;
    std::string header_line;
    while (std::getline(in,header_line)) {
        if (!trim(header_line).empty()) break;
    }
    if (header_line.empty()) return std::nullopt;
    const char delim=std::count(header_line.begin(),header_line.end(),';')>std::count(header_line.begin(),header_line.end(),',')?';':',';
    const auto header=split(header_line,delim);
    const int c_mjd=find_col(header,{"MJD","ModifiedJulianDate"});
    const int c_x=find_col(header,{"PM-x","x","xpole"});
    const int c_y=find_col(header,{"PM-y","y","ypole"});
    const int c_ut1=find_col(header,{"UT1-UTC","dut1","ut1utc"});
    const int c_lod=find_col(header,{"LOD"});
    const int c_dx=find_col(header,{"dX"});
    const int c_dy=find_col(header,{"dY"});
    const int c_type=find_col(header,{"Type","T"});
    if (c_mjd<0 || c_x<0 || c_y<0 || c_ut1<0) return std::nullopt;
    EopTable table;
    table.source=path;
    std::string line;
    while (std::getline(in,line)) {
        if (trim(line).empty()) continue;
        const auto row=split(line,delim);
        const auto mjd=at(row,c_mjd);
        const auto x=at(row,c_x);
        const auto y=at(row,c_y);
        const auto u=at(row,c_ut1);
        if (!mjd || !x || !y || !u) continue;
        EopRecord r;
        r.mjd=*mjd;
        r.xp_arcsec=*x;
        r.yp_arcsec=*y;
        r.dut1_s=*u;
        r.lod_ms=at(row,c_lod).value_or(0.0);
        r.dx_mas=at(row,c_dx).value_or(0.0);
        r.dy_mas=at(row,c_dy).value_or(0.0);
        if (c_type>=0 && static_cast<std::size_t>(c_type)<row.size()) {
            const auto t=key(row[static_cast<std::size_t>(c_type)]);
            r.prediction=t=="p" || t.find("pred")!=std::string::npos;
        }
        table.records.push_back(r);
    }
    std::sort(table.records.begin(),table.records.end(),[](const auto& a,const auto& b){ return a.mjd<b.mjd; });
    if (table.records.empty()) return std::nullopt;
    return table;
}

std::optional<EopRecord> nearest_eop(const EopTable& table,double mjd) {
    if (table.records.empty()) return std::nullopt;
    const auto it=std::lower_bound(table.records.begin(),table.records.end(),mjd,[](const auto& r,double v){ return r.mjd<v; });
    if (it==table.records.begin()) return *it;
    if (it==table.records.end()) return table.records.back();
    const auto prev=it-1;
    return std::abs(prev->mjd-mjd)<=std::abs(it->mjd-mjd)?*prev:*it;
}

std::optional<EopRecord> interpolate_eop(const EopTable& table,double mjd) {
    if (table.records.empty()) return std::nullopt;
    const auto it=std::lower_bound(table.records.begin(),table.records.end(),mjd,[](const auto& r,double v){ return r.mjd<v; });
    if (it==table.records.begin()) return *it;
    if (it==table.records.end()) return table.records.back();
    const auto a=it-1;
    const auto b=it;
    const double d=b->mjd-a->mjd;
    if (d<=0.0) return *a;
    const double t=(mjd-a->mjd)/d;
    EopRecord r;
    r.mjd=mjd;
    r.xp_arcsec=a->xp_arcsec+(b->xp_arcsec-a->xp_arcsec)*t;
    r.yp_arcsec=a->yp_arcsec+(b->yp_arcsec-a->yp_arcsec)*t;
    r.dut1_s=a->dut1_s+(b->dut1_s-a->dut1_s)*t;
    r.lod_ms=a->lod_ms+(b->lod_ms-a->lod_ms)*t;
    r.dx_mas=a->dx_mas+(b->dx_mas-a->dx_mas)*t;
    r.dy_mas=a->dy_mas+(b->dy_mas-a->dy_mas)*t;
    r.prediction=a->prediction || b->prediction;
    return r;
}

EopResolution resolve_eop(const EopTable& table,double mjd) {
    if (table.records.empty() || !std::isfinite(mjd) || mjd < table.records.front().mjd || mjd > table.records.back().mjd) return {};
    const auto record = interpolate_eop(table, mjd);
    if (!record) return {};
    const auto availability = record->prediction ? EopAvailability::Predicted : EopAvailability::Observed;
    return {*record, availability, 0.0};
}

}

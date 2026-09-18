#include <nadir/astro/omm.hpp>
#include <nadir/core/json.hpp>
#include <algorithm>
#include <cctype>

namespace nadir::astro {

static std::string lower(std::string s) {
    std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

static std::string str(const json::Value& v,const char* key) {
    const auto* p=v.get(key);
    return p?p->as_string():std::string{};
}

static double num(const json::Value& v,const char* key) {
    const auto* p=v.get(key);
    if (!p) return 0.0;
    if (p->is_number()) return p->as_number();
    if (p->is_string()) {
        try { return std::stod(p->as_string()); } catch (...) { return 0.0; }
    }
    return 0.0;
}

static std::uint64_t u64(const json::Value& v,const char* key) {
    const double n=num(v,key);
    return n>0.0?static_cast<std::uint64_t>(n):0;
}

static int i32(const json::Value& v,const char* key) {
    return static_cast<int>(num(v,key));
}

static std::optional<OmmRecord> record(const json::Value& v) {
    if (!v.is_object()) return std::nullopt;
    OmmRecord r;
    r.object_name=str(v,"OBJECT_NAME");
    r.object_id=str(v,"OBJECT_ID");
    r.epoch=str(v,"EPOCH");
    r.classification_type=str(v,"CLASSIFICATION_TYPE");
    r.center_name=str(v,"CENTER_NAME");
    r.ref_frame=str(v,"REF_FRAME");
    r.time_system=str(v,"TIME_SYSTEM");
    r.mean_element_theory=str(v,"MEAN_ELEMENT_THEORY");
    r.norad_cat_id=u64(v,"NORAD_CAT_ID");
    r.ephemeris_type=i32(v,"EPHEMERIS_TYPE");
    r.element_set_no=i32(v,"ELEMENT_SET_NO");
    r.rev_at_epoch=i32(v,"REV_AT_EPOCH");
    r.mean_motion_rev_day=num(v,"MEAN_MOTION");
    r.eccentricity=num(v,"ECCENTRICITY");
    r.inclination_deg=num(v,"INCLINATION");
    r.raan_deg=num(v,"RA_OF_ASC_NODE");
    r.arg_pericenter_deg=num(v,"ARG_OF_PERICENTER");
    r.mean_anomaly_deg=num(v,"MEAN_ANOMALY");
    r.bstar=num(v,"BSTAR");
    r.mean_motion_dot=num(v,"MEAN_MOTION_DOT");
    r.mean_motion_ddot=num(v,"MEAN_MOTION_DDOT");
    if (r.object_name.empty() && r.norad_cat_id==0) return std::nullopt;
    return r;
}

OmmParseResult parse_omm_json(const std::string& text) {
    const auto parsed=json::parse(text);
    if (!parsed.ok) return {false,{},parsed.error+" at "+std::to_string(parsed.offset)};
    std::vector<OmmRecord> out;
    if (const auto* a=parsed.value.as_array()) {
        out.reserve(a->size());
        for (const auto& v:*a) {
            auto r=record(v);
            if (r) out.push_back(std::move(*r));
        }
    } else {
        auto r=record(parsed.value);
        if (r) out.push_back(std::move(*r));
    }
    if (out.empty()) return {false,{},"no OMM records"};
    return {true,std::move(out),{}};
}

OmmParseResult load_omm_json(const std::string& path) {
    const auto text=json::read_text_file(path);
    if (!text) return {false,{},"cannot read "+path};
    return parse_omm_json(*text);
}

std::vector<OmmRecord> find_omm(const std::vector<OmmRecord>& records,const std::string& query,std::size_t limit) {
    std::vector<OmmRecord> out;
    const auto q=lower(query);
    for (const auto& r:records) {
        const bool match=q.empty() || lower(r.object_name).find(q)!=std::string::npos || lower(r.object_id).find(q)!=std::string::npos || std::to_string(r.norad_cat_id)==q;
        if (!match) continue;
        out.push_back(r);
        if (out.size()>=limit) break;
    }
    return out;
}

}

#include <nadir/astro/omm.hpp>
#include <nadir/core/json.hpp>
#include <algorithm>
#include <cctype>
#include <map>
#include <regex>
#include <sstream>

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

static std::optional<OmmRecord> record(const std::map<std::string, std::string>& fields) {
    const auto get = [&](const char* key) { const auto it = fields.find(key); return it == fields.end() ? std::string{} : it->second; };
    const auto number = [&](const char* key) { try { return std::stod(get(key)); } catch (...) { return 0.0; } };
    OmmRecord r;
    r.object_name=get("OBJECT_NAME"); r.object_id=get("OBJECT_ID"); r.epoch=get("EPOCH");
    r.classification_type=get("CLASSIFICATION_TYPE"); r.center_name=get("CENTER_NAME");
    r.ref_frame=get("REF_FRAME"); r.time_system=get("TIME_SYSTEM"); r.mean_element_theory=get("MEAN_ELEMENT_THEORY");
    r.norad_cat_id=static_cast<std::uint64_t>(number("NORAD_CAT_ID")); r.ephemeris_type=static_cast<int>(number("EPHEMERIS_TYPE"));
    r.element_set_no=static_cast<int>(number("ELEMENT_SET_NO")); r.rev_at_epoch=static_cast<int>(number("REV_AT_EPOCH"));
    r.mean_motion_rev_day=number("MEAN_MOTION"); r.eccentricity=number("ECCENTRICITY"); r.inclination_deg=number("INCLINATION");
    r.raan_deg=number("RA_OF_ASC_NODE"); r.arg_pericenter_deg=number("ARG_OF_PERICENTER"); r.mean_anomaly_deg=number("MEAN_ANOMALY");
    r.bstar=number("BSTAR"); r.mean_motion_dot=number("MEAN_MOTION_DOT"); r.mean_motion_ddot=number("MEAN_MOTION_DDOT");
    if (r.norad_cat_id == 0 || r.epoch.empty() || r.mean_motion_rev_day <= 0.0 || r.eccentricity < 0.0 || r.eccentricity >= 1.0) return std::nullopt;
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

OmmParseResult parse_omm_kvn(const std::string& text) {
    std::map<std::string, std::string> fields;
    std::istringstream input(text);
    for (std::string line; std::getline(input, line); ) {
        const auto equals = line.find('=');
        if (equals == std::string::npos) continue;
        auto key = line.substr(0, equals); auto value = line.substr(equals + 1);
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
        const auto first = value.find_first_not_of(" \t\r");
        const auto last = value.find_last_not_of(" \t\r");
        fields[key] = first == std::string::npos ? "" : value.substr(first, last - first + 1);
    }
    const auto parsed = record(fields);
    return parsed ? OmmParseResult{true, {*parsed}, {}} : OmmParseResult{false, {}, "invalid OMM KVN"};
}

OmmParseResult parse_omm_xml(const std::string& text) {
    std::map<std::string, std::string> fields;
    const std::regex tag(R"(<([A-Z_]+)>([^<]*)</\1>)");
    for (auto it = std::sregex_iterator(text.begin(), text.end(), tag); it != std::sregex_iterator(); ++it)
        fields[(*it)[1].str()] = (*it)[2].str();
    const auto parsed = record(fields);
    return parsed ? OmmParseResult{true, {*parsed}, {}} : OmmParseResult{false, {}, "invalid OMM XML"};
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

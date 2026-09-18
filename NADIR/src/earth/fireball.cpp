#include <nadir/earth/fireball.hpp>
#include <nadir/core/json.hpp>
#include <cstdlib>
#include <map>

namespace nadir::earth {

static std::optional<double> number(const json::Value& v) {
    if (v.is_number()) return v.as_number();
    if (!v.is_string()) return std::nullopt;
    const auto s=v.as_string();
    if (s.empty()) return std::nullopt;
    char* end=nullptr;
    const double x=std::strtod(s.c_str(),&end);
    if (!end || *end!='\0') return std::nullopt;
    return x;
}

std::optional<FireballFeed> parse_jpl_fireballs(const std::string& text) {
    const auto p=json::parse(text);
    if (!p.ok || !p.value.is_object()) return std::nullopt;
    FireballFeed feed;
    if (const auto* sig=p.value.get("signature")) if (const auto* v=sig->get("version")) feed.version=v->as_string();
    const auto* fields=p.value.get("fields");
    const auto* data=p.value.get("data");
    if (!fields || !data || !fields->is_array() || !data->is_array()) return feed;
    std::map<std::string,std::size_t> idx;
    const auto& h=*fields->as_array();
    for (std::size_t i=0;i<h.size();++i) idx[h[i].as_string()]=i;
    auto val=[&](const json::Array& row,const std::string& key)->const json::Value* {
        const auto it=idx.find(key);
        return it==idx.end() || it->second>=row.size()?nullptr:&row[it->second];
    };
    for (const auto& item:*data->as_array()) {
        const auto* row=item.as_array();
        if (!row) continue;
        Fireball f;
        if (const auto* v=val(*row,"date")) f.date=v->as_string();
        if (const auto* v=val(*row,"lat")) f.latitude_deg=number(*v);
        if (const auto* v=val(*row,"lat-dir"); v && f.latitude_deg && v->as_string()=="S") *f.latitude_deg=-*f.latitude_deg;
        if (const auto* v=val(*row,"lon")) f.longitude_deg=number(*v);
        if (const auto* v=val(*row,"lon-dir"); v && f.longitude_deg && v->as_string()=="W") *f.longitude_deg=-*f.longitude_deg;
        if (const auto* v=val(*row,"alt")) f.altitude_km=number(*v);
        if (const auto* v=val(*row,"vel")) f.velocity_km_s=number(*v);
        if (const auto* v=val(*row,"energy")) f.radiated_energy_1e10_j=number(*v);
        if (const auto* v=val(*row,"impact-e")) f.impact_energy_kt=number(*v);
        feed.events.push_back(std::move(f));
    }
    return feed;
}

}

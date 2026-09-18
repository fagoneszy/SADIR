#include <nadir/earth/seismic.hpp>
#include <nadir/core/json.hpp>

namespace nadir::earth {

static std::string str(const json::Value* v,std::string fallback={}) { return v?v->as_string(std::move(fallback)):std::string{}; }
static double num(const json::Value* v,double fallback=0.0) { return v?v->as_number(fallback):fallback; }

EarthquakeParseResult parse_usgs_geojson(const std::string& text) {
    const auto parsed=json::parse(text);
    if (!parsed.ok) return {false,{},parsed.error};
    EarthquakeFeed feed;
    if (const auto* meta=parsed.value.get("metadata")) {
        feed.title=str(meta->get("title"));
        feed.generated_ms=static_cast<std::int64_t>(num(meta->get("generated")));
    }
    const auto* features=parsed.value.get("features");
    if (!features || !features->is_array()) return {false,{},"missing features"};
    for (const auto& f:*features->as_array()) {
        Earthquake q;
        q.id=str(f.get("id"));
        if (const auto* p=f.get("properties")) {
            q.place=str(p->get("place"));
            q.url=str(p->get("url"));
            q.magnitude=num(p->get("mag"));
            q.time_ms=static_cast<std::int64_t>(num(p->get("time")));
            q.updated_ms=static_cast<std::int64_t>(num(p->get("updated")));
        }
        if (const auto* g=f.get("geometry")) {
            if (const auto* c=g->get("coordinates"); c && c->is_array()) {
                const auto& a=*c->as_array();
                if (a.size()>0) q.longitude_deg=a[0].as_number();
                if (a.size()>1) q.latitude_deg=a[1].as_number();
                if (a.size()>2) q.depth_km=a[2].as_number();
            }
        }
        if (!q.id.empty()) feed.events.push_back(std::move(q));
    }
    return {true,std::move(feed),{}};
}

EarthquakeParseResult load_usgs_geojson(const std::string& path) {
    const auto text=json::read_text_file(path);
    if (!text) return {false,{},"cannot read "+path};
    return parse_usgs_geojson(*text);
}

}

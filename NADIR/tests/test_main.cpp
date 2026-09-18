#include <nadir/astro/horizons.hpp>
#include <nadir/astro/omm.hpp>
#include <nadir/core/json.hpp>
#include <nadir/core/sha256.hpp>
#include <nadir/core/time.hpp>
#include <nadir/earth/fireball.hpp>
#include <nadir/earth/seismic.hpp>
#include <nadir/geo/eop.hpp>
#include <nadir/geo/frames.hpp>
#include <nadir/geo/wgs84.hpp>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

int main() {
    const nadir::geo::Geodetic g{0.0,0.0,0.0};
    const auto e=nadir::geo::geodetic_to_ecef(g);
    if (std::abs(e.x-nadir::geo::a)>0.001) return 1;
    if (std::abs(e.y)>0.001||std::abs(e.z)>0.001) return 2;
    const auto back=nadir::geo::ecef_to_geodetic(e);
    if (std::abs(back.latitude_deg)>1e-8||std::abs(back.longitude_deg)>1e-8) return 3;
    if (nadir::core::sha256("abc")!="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") return 4;

    const std::string h="x\n$$SOE\n2460000.5, A.D. 2023-Jan-01 00:00:00.0000, 1, 2, 3, 4, 5, 6, 7, 8, 9,\n$$EOE\ny";
    const auto hs=nadir::astro::parse_horizons_vector_csv(h);
    if (!hs) return 5;
    if (hs->range_km!=8.0||hs->range_rate_km_s!=9.0) return 6;

    const auto j=nadir::json::parse(R"({"a":1,"b":[true,"x"],"c":null})");
    if (!j.ok || !j.value.get("a") || j.value.get("a")->as_number()!=1.0) return 7;
    if (!j.value.get("b") || !j.value.get("b")->is_array()) return 8;

    const std::string omm=R"([{"OBJECT_NAME":"TEST-100701","OBJECT_ID":"2026-001A","EPOCH":"2026-09-17T00:00:00.000000","NORAD_CAT_ID":"100701","MEAN_MOTION":"15.5","ECCENTRICITY":"0.001","INCLINATION":"51.6","RA_OF_ASC_NODE":"20.0","ARG_OF_PERICENTER":"30.0","MEAN_ANOMALY":"40.0","BSTAR":"0.0001","MEAN_ELEMENT_THEORY":"SGP4"}])";
    const auto op=nadir::astro::parse_omm_json(omm);
    if (!op.ok || op.records.size()!=1) return 9;
    if (op.records[0].norad_cat_id!=100701ULL) return 10;
    if (nadir::astro::find_omm(op.records,"100701",5).size()!=1) return 11;

    const auto t=nadir::core::time_from_unix_ns(1483228800LL*1000000000LL,0.25);
    if (t.tai_minus_utc_s!=37) return 12;
    if (std::abs((t.jd_ut1-t.jd_utc)*86400.0-0.25)>1e-4) return 13;

    const auto tmp=std::filesystem::temp_directory_path()/"nadir_eop_test.csv";
    {
        std::ofstream out(tmp);
        out<<"MJD,PM-x,PM-y,UT1-UTC,LOD,dX,dY,Type\n";
        out<<"60000,0.1,0.2,-0.1,0.5,0.01,0.02,I\n";
        out<<"60001,0.3,0.4,-0.3,0.7,0.03,0.04,P\n";
    }
    const auto et=nadir::geo::load_iers_csv(tmp.string());
    std::filesystem::remove(tmp);
    if (!et || et->records.size()!=2) return 14;
    const auto ei=nadir::geo::interpolate_eop(*et,60000.5);
    if (!ei || std::abs(ei->dut1_s+0.2)>1e-12 || !ei->prediction) return 15;

    const std::string eq=R"({"metadata":{"title":"x","generated":1},"features":[{"id":"q1","properties":{"place":"Somewhere","mag":5.2,"time":2,"updated":3,"url":"u"},"geometry":{"coordinates":[-45.0,-12.0,10.0]}}]})";
    const auto eqp=nadir::earth::parse_usgs_geojson(eq);
    if (!eqp.ok || eqp.feed.events.size()!=1 || std::abs(eqp.feed.events[0].magnitude-5.2)>1e-12) return 16;

    const std::string fb=R"({"signature":{"version":"1.0"},"fields":["date","lat","lat-dir","lon","lon-dir","alt","vel","energy","impact-e"],"data":[["2026-01-01","12","S","45","W","30","18","2","0.1"]]})";
    const auto fbp=nadir::earth::parse_jpl_fireballs(fb);
    if (!fbp || fbp->events.size()!=1) return 17;
    if (!fbp->events[0].latitude_deg || std::abs(*fbp->events[0].latitude_deg+12.0)>1e-12) return 18;
    if (!fbp->events[0].longitude_deg || std::abs(*fbp->events[0].longitude_deg+45.0)>1e-12) return 19;

    const double dop=nadir::geo::doppler_observed_hz(145800000.0,-1000.0);
    if (!(dop>145800000.0)) return 20;

    std::cout<<"OK\n";
    return 0;
}

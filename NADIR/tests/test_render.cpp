#include <nadir/math/vec3.hpp>
#include <nadir/math/quat.hpp>
#include <nadir/render/camera.hpp>
#include <nadir/render/projection.hpp>
#include <cmath>
#include <iostream>

int main() {
    using nadir::math::Vec3d;
    using nadir::math::Quatd;

    {
        Vec3d a{1.0, 2.0, 3.0};
        Vec3d b{4.0, -1.0, 0.5};
        Vec3d c = a + b;
        if (std::abs(c.x - 5.0) > 1e-12 || std::abs(c.y - 1.0) > 1e-12 || std::abs(c.z - 3.5) > 1e-12) return 10;
        Vec3d d = a - b;
        if (std::abs(d.x + 3.0) > 1e-12) return 11;
        if (std::abs(a.dot(b) - (4.0 -2.0 + 1.5)) > 1e-12) return 12;
        Vec3d e = a.cross(b);
        if (!std::isfinite(e.x) || !std::isfinite(e.y) || !std::isfinite(e.z)) return 13;
        Vec3d f{3.0, 4.0, 0.0};
        if (std::abs(f.norm() - 5.0) > 1e-12) return 14;
        Vec3d g = f.normalized();
        if (std::abs(g.norm() - 1.0) > 1e-12) return 15;
        Vec3d h{};
        auto hn = h.normalized();
        if (hn.x != 0.0 || hn.y != 0.0 || hn.z != 0.0) return 16;
        if (std::isnan(g.x) || std::isnan(g.y) || std::isnan(g.z)) return 17;
    }

    {
        Quatd q = Quatd::identity();
        if (std::abs(q.w - 1.0) > 1e-12 || q.x != 0.0 || q.y != 0.0 || q.z != 0.0) return 20;
        Quatd axis{std::sqrt(2.0)/2, std::sqrt(2.0)/2, 0.0};
        Quatd r = Quatd::from_axis_angle({0.0, 0.0, 1.0}, 3.14159265358979323846 * 0.5);
        Vec3d v{1.0, 0.0, 0.0};
        Vec3d vr = r.rotate(v);
        if (std::abs(vr.x - 0.0) > 1e-9 || std::abs(vr.y - 1.0) > 1e-9) return 21;
        Quatd q2{2.0, 0.0, 0.0, 0.0};
        auto qn = q2.normalized();
        if (std::abs(qn.w - 1.0) > 1e-12) return 22;
        Quatd a = Quatd::from_axis_angle({1.0, 0.0, 0.0}, 0.5);
        Quatd b = Quatd::from_axis_angle({0.0, 1.0, 0.0}, 0.3);
        auto c = a * b;
        if (!std::isfinite(c.w)) return 23;
    }

    {
        nadir::render::Camera cam{};
        auto pos0 = cam.position();
        double r0 = pos0.norm();
        cam.yaw(1.0);
        auto pos1 = cam.position();
        double r1 = pos1.norm();
        if (std::abs(r0 - r1) > 1e-9) return 30;
        nadir::render::Camera cam2{};
        auto p0 = cam2.position();
        double r2 = p0.norm();
        cam2.pitch(0.7);
        auto p1 = cam2.position();
        if (std::abs(p1.norm() - r2) > 1e-9) return 31;
        nadir::render::Camera cam3{};
        auto t0 = cam3.target;
        cam3.roll(0.5);
        if (cam3.target.x != t0.x || cam3.target.y != t0.y || cam3.target.z != t0.z) return 32;
        nadir::render::Camera cam4{};
        double d0 = cam4.distance;
        cam4.zoom(1.0);
        if (!(cam4.distance < d0)) return 33;
        if (!std::isfinite(cam4.distance) || cam4.distance <= 0) return 34;
    }

    {
        nadir::math::Vec3d point{0.0, 0.0, -5.0};
        auto proj = nadir::render::project_perspective(point, 60.0, 16.0/9.0, 0.01, 1000.0);
        if (!proj.visible) return 40;
        if (std::abs(proj.x_ndc) > 1e-12) return 41;
        if (std::abs(proj.y_ndc) > 1e-12) return 42;
    }

    {
        nadir::math::Vec3d left{-1.0, 0.0, -5.0};
        nadir::math::Vec3d right{1.0, 0.0, -5.0};
        auto pl = nadir::render::project_perspective(left, 60.0, 1.0, 0.01, 1000.0);
        auto pr = nadir::render::project_perspective(right, 60.0, 1.0, 0.01, 1000.0);
        if (!pl.visible || !pr.visible) return 43;
        if (!(pl.x_ndc < 0.0 && pr.x_ndc > 0.0)) return 44;
        if (!(pl.x_ndc < pr.x_ndc)) return 45;
    }

    {
        nadir::math::Vec3d up{0.0, 1.0, -5.0};
        nadir::math::Vec3d down{0.0, -1.0, -5.0};
        auto pu = nadir::render::project_perspective(up, 60.0, 1.0, 0.01, 1000.0);
        auto pd = nadir::render::project_perspective(down, 60.0, 1.0, 0.01, 1000.0);
        if (!pu.visible || !pd.visible) return 46;
        if (!(pu.y_ndc > 0.0 && pd.y_ndc < 0.0)) return 47;
    }

    {
        nadir::math::Vec3d near_pt{0.0, 0.0, -0.005};
        auto pn = nadir::render::project_perspective(near_pt, 60.0, 1.0, 0.01, 1000.0);
        if (pn.visible) return 48;
    }

    {
        nadir::math::Vec3d far_pt{0.0, 0.0, -2000.0};
        auto pf = nadir::render::project_perspective(far_pt, 60.0, 1.0, 0.01, 1000.0);
        if (pf.visible) return 49;
    }

    {
        nadir::math::Vec3d behind{0.0, 0.0, 5.0};
        auto pb = nadir::render::project_perspective(behind, 60.0, 1.0, 0.01, 1000.0);
        if (pb.visible) return 50;
    }

    {
        nadir::math::Vec3d center{0.0, 0.0, -5.0};
        auto proj = nadir::render::project_perspective(center, 60.0, 16.0/9.0, 0.01, 1000.0);
        auto sp = nadir::render::ndc_to_viewport(proj, 320, 200);
        if (!sp.visible) return 51;
        if (std::abs(sp.x - 160.0) > 2.0) return 52;
        if (std::abs(sp.y - 100.0) > 2.0) return 53;
    }

    {
        nadir::math::Vec3d p{0.0, 0.0, -5.0};
        auto r1 = nadir::render::project_perspective(p, 179.0, 1.0, 0.01, 1000.0);
        auto r2 = nadir::render::project_perspective(p, 10.0, 1.0, 0.01, 1000.0);
        if (!std::isfinite(r1.x_ndc) || !std::isfinite(r1.y_ndc)) return 54;
        if (!std::isfinite(r2.x_ndc) || !std::isfinite(r2.y_ndc)) return 55;
    }

    {
        nadir::math::Vec3d nan_pt{std::numeric_limits<double>::quiet_NaN(), 0.0, -5.0};
        auto pn = nadir::render::project_perspective(nan_pt, 60.0, 1.0, 0.01, 1000.0);
        if (pn.visible) return 56;
        nadir::math::Vec3d inf_pt{std::numeric_limits<double>::infinity(), 0.0, -5.0};
        auto pi = nadir::render::project_perspective(inf_pt, 60.0, 1.0, 0.01, 1000.0);
        if (pi.visible) return 57;
    }

    std::cout << "render tests PASS\n";
    return 0;
}

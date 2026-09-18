#pragma once
#include <array>
#include <cmath>
#include <cstddef>

namespace nadir {

struct Vec2 {
    double x{};
    double y{};
};

struct Vec3 {
    double x{};
    double y{};
    double z{};

    Vec3 operator+() const { return *this; }
    Vec3 operator-() const { return {-x,-y,-z}; }
    Vec3 operator+(const Vec3& rhs) const { return {x+rhs.x,y+rhs.y,z+rhs.z}; }
    Vec3 operator-(const Vec3& rhs) const { return {x-rhs.x,y-rhs.y,z-rhs.z}; }
    Vec3 operator*(double s) const { return {x*s,y*s,z*s}; }
    Vec3 operator/(double s) const { return {x/s,y/s,z/s}; }
    Vec3& operator+=(const Vec3& rhs) { x+=rhs.x; y+=rhs.y; z+=rhs.z; return *this; }
    Vec3& operator-=(const Vec3& rhs) { x-=rhs.x; y-=rhs.y; z-=rhs.z; return *this; }
};

inline Vec3 operator*(double s,const Vec3& v) { return v*s; }
inline double dot(const Vec3& a,const Vec3& b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
inline Vec3 cross(const Vec3& a,const Vec3& b) { return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
inline double length2(const Vec3& v) { return dot(v,v); }
inline double length(const Vec3& v) { return std::sqrt(length2(v)); }
inline Vec3 normalize(const Vec3& v) { const double l=length(v); return l==0.0?Vec3{}:v/l; }
inline Vec3 lerp(const Vec3& a,const Vec3& b,double t) { return a+(b-a)*t; }

struct Mat3 {
    std::array<double,9> m{};

    static Mat3 identity() { return {{1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0}}; }
    static Mat3 rotation_x(double a) { const double c=std::cos(a); const double s=std::sin(a); return {{1.0,0.0,0.0,0.0,c,-s,0.0,s,c}}; }
    static Mat3 rotation_y(double a) { const double c=std::cos(a); const double s=std::sin(a); return {{c,0.0,s,0.0,1.0,0.0,-s,0.0,c}}; }
    static Mat3 rotation_z(double a) { const double c=std::cos(a); const double s=std::sin(a); return {{c,-s,0.0,s,c,0.0,0.0,0.0,1.0}}; }

    Vec3 operator*(const Vec3& v) const {
        return {m[0]*v.x+m[1]*v.y+m[2]*v.z,m[3]*v.x+m[4]*v.y+m[5]*v.z,m[6]*v.x+m[7]*v.y+m[8]*v.z};
    }

    Mat3 operator*(const Mat3& r) const {
        Mat3 out{};
        for (std::size_t row=0;row<3;++row) for (std::size_t col=0;col<3;++col) for (std::size_t k=0;k<3;++k) out.m[row*3+col]+=m[row*3+k]*r.m[k*3+col];
        return out;
    }

    Mat3 transpose() const { return {{m[0],m[3],m[6],m[1],m[4],m[7],m[2],m[5],m[8]}}; }
};

struct Quaternion {
    double w{1.0};
    double x{};
    double y{};
    double z{};

    static Quaternion axis_angle(Vec3 axis,double angle) {
        axis=normalize(axis);
        const double h=angle*0.5;
        const double s=std::sin(h);
        return {std::cos(h),axis.x*s,axis.y*s,axis.z*s};
    }

    Quaternion normalized() const {
        const double n=std::sqrt(w*w+x*x+y*y+z*z);
        return n==0.0?Quaternion{}:Quaternion{w/n,x/n,y/n,z/n};
    }

    Quaternion conjugate() const { return {w,-x,-y,-z}; }

    Quaternion operator*(const Quaternion& q) const {
        return {w*q.w-x*q.x-y*q.y-z*q.z,w*q.x+x*q.w+y*q.z-z*q.y,w*q.y-x*q.z+y*q.w+z*q.x,w*q.z+x*q.y-y*q.x+z*q.w};
    }

    Vec3 rotate(const Vec3& v) const {
        const Quaternion q=normalized();
        const Quaternion p{0.0,v.x,v.y,v.z};
        const Quaternion r=q*p*q.conjugate();
        return {r.x,r.y,r.z};
    }

    Mat3 matrix() const {
        const auto q=normalized();
        const double xx=q.x*q.x, yy=q.y*q.y, zz=q.z*q.z;
        const double xy=q.x*q.y, xz=q.x*q.z, yz=q.y*q.z;
        const double wx=q.w*q.x, wy=q.w*q.y, wz=q.w*q.z;
        return {{1.0-2.0*(yy+zz),2.0*(xy-wz),2.0*(xz+wy),2.0*(xy+wz),1.0-2.0*(xx+zz),2.0*(yz-wx),2.0*(xz-wy),2.0*(yz+wx),1.0-2.0*(xx+yy)}};
    }
};

constexpr double pi=3.141592653589793238462643383279502884;
constexpr double deg_to_rad=pi/180.0;
constexpr double rad_to_deg=180.0/pi;

}

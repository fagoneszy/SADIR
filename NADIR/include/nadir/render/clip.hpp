#pragma once
#include <nadir/math/vec3.hpp>
namespace nadir::render { struct ClippedSegment3D { math::Vec3d a{},b{};bool visible{};}; struct NdcPoint { double x{},y{},inverse_depth{};bool visible{};}; struct ClippedNdcSegment { NdcPoint a{},b{};bool visible{},clipped{};}; ClippedSegment3D clip_depth(const math::Vec3d&,const math::Vec3d&,double near,double far) noexcept; ClippedNdcSegment clip_ndc(NdcPoint,NdcPoint) noexcept; }

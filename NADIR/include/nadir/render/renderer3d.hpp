#pragma once
#include <span>
#include <string>
#include <vector>
#include <nadir/render/camera.hpp>
#include <nadir/render/depth_buffer.hpp>
#include <nadir/render/display_transform.hpp>
#include <nadir/render/phosphor_buffer.hpp>
#include <nadir/render/render_stats.hpp>
#include <nadir/render/scene.hpp>
namespace nadir::render { struct ProjectedLabel { std::uint64_t entity_id{};int x{},y{};double depth{};int priority{};std::string text;}; class Renderer3D { public: Renderer3D(int w,int h):depth_(w,h),phosphor_(w,h){} void resize(int w,int h){depth_.resize(w,h);phosphor_.resize(w,h);} bool render(const SceneSnapshot&,const Camera&,const DisplayTransform&,double dt); const PhosphorBuffer& phosphor()const noexcept{return phosphor_;} std::span<const ProjectedLabel> labels()const noexcept{return labels_;} const RenderStats& stats()const noexcept{return stats_;} private: DepthBuffer depth_;PhosphorBuffer phosphor_;std::vector<ProjectedLabel> labels_;RenderStats stats_{};}; }

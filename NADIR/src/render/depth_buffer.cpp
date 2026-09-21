#include <nadir/render/depth_buffer.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
namespace nadir::render { void DepthBuffer::resize(int w,int h){w_=std::max(0,w);h_=std::max(0,h);values_.assign(static_cast<size_t>(w_)*h_,std::numeric_limits<double>::infinity());} void DepthBuffer::clear(){std::fill(values_.begin(),values_.end(),std::numeric_limits<double>::infinity());} bool DepthBuffer::test_and_write(int x,int y,double d){if(x<0||y<0||x>=w_||y>=h_||!std::isfinite(d)||d<=0)return false;auto& v=values_[static_cast<size_t>(y)*w_+x];if(d>=v)return false;v=d;return true;} double DepthBuffer::get(int x,int y)const{return x<0||y<0||x>=w_||y>=h_?std::numeric_limits<double>::infinity():values_[static_cast<size_t>(y)*w_+x];} }

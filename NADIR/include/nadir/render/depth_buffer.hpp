#pragma once
#include <vector>
namespace nadir::render { class DepthBuffer { public: DepthBuffer()=default; DepthBuffer(int w,int h){resize(w,h);} void resize(int w,int h); void clear(); bool test_and_write(int x,int y,double d); double get(int x,int y) const; int width() const noexcept{return w_;} int height() const noexcept{return h_;} private:int w_{},h_{};std::vector<double> values_;}; }

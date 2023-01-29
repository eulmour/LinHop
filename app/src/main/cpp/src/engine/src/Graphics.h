#ifndef ENGINE_GRAPHICS_H
#define ENGINE_GRAPHICS_H
#include "Framework.h"

namespace wuh {

class Graphics {

public:
    Graphics& init();
    Graphics& clear(Color color);
    Graphics& viewport(IVec2 size);
    Graphics& size(IVec2 size) { size_ = size; return *this; }
    [[nodiscard]] const IVec2& size() const { return size_; }
    [[nodiscard]] const IVec2& viewport() const { return viewport_; }
    [[nodiscard]] float scale() const { return viewport_[0] / size_[0]; }
    static void catch_error();

private:
    IVec2 viewport_;
    IVec2 size_;
};

} // end of namespace wuh

#endif //ENGINE_GRAPHICS_H
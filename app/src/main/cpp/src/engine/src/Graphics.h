#ifndef ENGINE_GRAPHICS_H
#define ENGINE_GRAPHICS_H
#include "Framework.h"

class Graphics {

public:
    Graphics& init();
    Graphics& clear(Color color);
    Graphics& viewport(IVec2 size);
    const IVec2& viewport() const;

private:
    IVec2 viewport_;
};

#endif //ENGINE_GRAPHICS_H

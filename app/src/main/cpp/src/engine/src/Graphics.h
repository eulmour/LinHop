#ifndef ENGINE_GRAPHICS_H
#define ENGINE_GRAPHICS_H
#include "Framework.h"

class Graphics {

public:
    Graphics& init();
    Graphics& clear(Color color);
    Graphics& viewport(IVec2 size);
};

#endif //ENGINE_GRAPHICS_H

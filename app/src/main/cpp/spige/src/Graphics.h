#ifndef SPIGE_GRAPHICS_H
#define SPIGE_GRAPHICS_H
#include "Framework.h"

class Graphics {

public:
    Graphics& init();
    Graphics& clear(Color color);
    Graphics& viewport(IVec2 size);
};

#endif //SPIGE_GRAPHICS_H

#ifndef SPIGE_SCENE_HPP
#define SPIGE_SCENE_HPP

#include "spige.h"

struct scene {
    GLsizei width;
    GLsizei height;

    vec4 background;
    struct tri tri;
    struct line line;
    struct text text;
    struct rect rect;

    struct audio audio_engine;
    struct audio_source audio_main;
    struct audio_source audio_alt;
    struct audio_source audio_bounce;
    struct audio_source audio_fail_a;
    struct audio_source audio_fail_b;
    struct audio_source audio_warning;
};

void scene_init(struct scene* p_scene);
void scene_draw(struct scene* p_scene);
void scene_unload(struct scene* p_scene);

#endif //SPIGE_SCENE_HPP

#include "c_scene.h"
#include "memory.h"

void scene_init(struct scene* p_scene) {

    memcpy(p_scene->background, (vec4) {0.4f, 0.25f, 0.12f, 1.f}, sizeof(vec4));

    tri_load(&p_scene->tri);
    memcpy(p_scene->tri.scale, (vec2){ 100.f, 100.f }, sizeof(vec2));
    memcpy(p_scene->tri.color, (vec4){ 0.8f, 0.5f, 0.2f, 1.0f }, sizeof(vec4));

    line_load(&p_scene->line);
    p_scene->line.width = 5.f;
    text_load(&p_scene->text, "fonts/OCRAEXT.TTF", 48.f);

    rect_load(&p_scene->rect);
    rect_use_texture(&p_scene->rect, texture_load("textures/circle.png"));
    memcpy(p_scene->rect.color, (vec4){ 1.f, 1.f, 1.f, 1.f }, sizeof(vec4));
    memcpy(p_scene->rect.scale, (vec2){ 300.f, 300.f }, sizeof(vec2));

    audio_init(&p_scene->audio_engine);

    audio_source_load(&p_scene->audio_main, "audio/a.wav", 1.f);
    audio_source_load(&p_scene->audio_alt, "audio/b.wav", 1.f);
    audio_source_load(&p_scene->audio_bounce, "audio/bounce.wav", 1.f);
    audio_source_load(&p_scene->audio_fail_a, "audio/fail.wav", 1.f);
    audio_source_load(&p_scene->audio_fail_b, "audio/fail2.wav", 1.f);
    audio_source_load(&p_scene->audio_warning, "audio/warning.wav", 1.f);

    audio_play(&p_scene->audio_engine, &p_scene->audio_main);
}

static void scene_update(struct scene* p_scene) {
//    LOGI("scene_update is not implemented\n");
}

void mangle(GLfloat* val, GLfloat* amount) {
    *val += *amount;

    if (*val > 1.f || *val < -1.f)
        *amount = -*amount;
}

void scene_draw(struct scene* p_scene) {

    scene_update(p_scene);

    tri_draw(&p_scene->tri, (vec2){static_cast<float>(p_scene->width/2), static_cast<float>(p_scene->height/2)});
    line_draw(&p_scene->line, (vec4){0.f, 0.f, 500.f, 700.f});

    static float amount = 0.05f;
    mangle(&p_scene->text.color[1], &amount);
    text_draw(&p_scene->text, "aBcDhIfe23V:@#%", (vec2){200.f, 400.f});

    rect_draw(&p_scene->rect, (vec2){ 100.f, 100.f });

#ifdef DEBUG
    checkError();
#endif
}

void scene_unload(struct scene* p_scene) {
    tri_unload(&p_scene->tri);
    line_unload(&p_scene->line);
    text_unload(&p_scene->text);
    rect_unload(&p_scene->rect);
}
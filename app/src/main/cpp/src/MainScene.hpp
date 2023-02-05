#ifndef LINHOP_MAINSCENE_HPP
#define LINHOP_MAINSCENE_HPP

#include <memory>

#include "engine/engine.h"
#include "Util.hpp"
#include "Lines.hpp"
#include "Ball.hpp"
#include "Tail.hpp"
#include "Sparks.hpp"
#include "Lasers.hpp"
#include "Label.hpp"

class MainScene : public wuh::Activity {
public:
    MainScene() = delete;
    explicit MainScene(wuh::Engine& e);

    ~MainScene() override;

    void suspend(wuh::Engine& e) override;
    void resume(wuh::Engine& e) override;
    void render(wuh::Engine& e) override;
    const char* title() override { return "LinHop"; }
    bool update(wuh::Engine& e);
    bool input(wuh::Engine& e);

    struct Resources {};

    std::unique_ptr<Resources> res;

    std::unique_ptr<wuh::Audio> audio_engine;
    std::unique_ptr<wuh::Audio::Source> audio_main;
    std::unique_ptr<wuh::Audio::Source> audio_alt;
    std::unique_ptr<wuh::Audio::Source> audio_bounce;
    std::unique_ptr<wuh::Audio::Source> audio_fail_a;
    std::unique_ptr<wuh::Audio::Source> audio_fail_b;
    std::unique_ptr<wuh::Audio::Source> audio_warning;

    // drawables
    std::unique_ptr<wuh::Line> line;
    std::unique_ptr<wuh::Text> small_text;
    std::unique_ptr<wuh::Text> medium_text;
    std::unique_ptr<wuh::Text> large_text;

    // classes
    std::unique_ptr<Lines> lines, rand_lines;
    std::unique_ptr<Ball> ball;
    std::unique_ptr<Lasers> lasers;
    std::unique_ptr<Sparks> sparks;
    std::unique_ptr<Tail> ball_tail, cursor_tail;

    // labels
    std::unique_ptr<Label> label_menu_title;
    std::unique_ptr<Label> label_menu_continue;
    std::unique_ptr<Label> label_menu_start;
    std::unique_ptr<Label> label_menu_settings;
    std::unique_ptr<Label> label_menu_exit;
    // std::unique_ptr<Label> label_menu_hint;
    std::unique_ptr<Label> label_menu_mode;

    std::unique_ptr<Label> label_endgame_restart;
    std::unique_ptr<Label> label_endgame_score;

    std::unique_ptr<Label> label_settings_title;
    std::unique_ptr<Label> label_settings_fx;
    std::unique_ptr<Label> label_settings_music_volume;
    std::unique_ptr<Label> label_settings_unlock_resizing;
    std::unique_ptr<Label> label_settings_reset_statistics;
    std::unique_ptr<Label> label_settings_log;
    std::unique_ptr<Label> label_settings_back;

    std::unique_ptr<Label> label_game_score;
    std::unique_ptr<Label> label_game_fps;

    enum class GameState {
        MENU, SETTINGS, INGAME, PAUSED, ENDGAME, EXITING, END
    } game_state = GameState::MENU;

    enum class GameMode {
        CLASSIC, HIDDEN, END
    } game_mode = GameMode::CLASSIC;

    enum class MenuSelected {
        CONTINUE, START, SETTINGS, EXIT, END
    } menu_selected = MenuSelected::START;

    enum class SettingsSelected {
        FX_ENABLED, MUSIC_VOLUME, RESET_STATISTICS, LOG, BACK, END
    } settings_selected = SettingsSelected::FX_ENABLED;

    bool onEventPointerMove(wuh::Engine& e);
    bool onEventPointerDown();
    bool onEventPointerUp(wuh::Engine& e);
    bool onEventSelect(wuh::Engine& e);
    void onEventUp();
    void onEventLeft();
    void onEventDown();
    void onEventRight();
    void onEventBack(wuh::Engine& e);

private:
    void reset(wuh::Engine& e);

    glm::vec2 prev_mouse_pos {0};
    glm::vec2 last_click {0};
    wuh::Color background_color{0.0f, 0.1f, 0.2f, 1.f};

    struct SaveData {
        long max_score_classic;
        long max_score_hidden;
        long fx_enabled;
        long unlock_resizing;
        float music_volume_float;
    } save_data { 0, 0, 1, 0, .8f };

    float min_x{};
    float max_x{};
    float area_width{ 450.f };
    float area_height{ 800.f };
    float ratio{ area_width / area_height };

    constexpr static float required_ratio{ 450.f / 800.f };
    static constexpr float ball_strength_mod{ 25000 };
    static constexpr float ball_gravity_mod{ 8000 };
    static constexpr float rand_lines_density{ 350.f }; // lower = higher
    static constexpr float small_text_size{ 28.f };
    static constexpr float medium_text_size{ 64.f };
    static constexpr float large_text_size{ 90.f };

    float last_place = rand_lines_density;
    long game_score = 0L;

protected:
    bool pressed {false};
    bool pressed_once {false};
};

// for each type you want the operator(s) to be enabled, do this:
template<>
struct my_enum_is_unit_steppable<MainScene::GameState> {
    enum {
        value = true
    };
};
template<>
struct my_enum_is_unit_steppable<MainScene::GameMode> {
    enum {
        value = true
    };
};
template<>
struct my_enum_is_unit_steppable<MainScene::MenuSelected> {
    enum {
        value = true
    };
};
template<>
struct my_enum_is_unit_steppable<MainScene::SettingsSelected> {
    enum {
        value = true
    };
};

#endif //LINHOP_MAINSCENE_HPP

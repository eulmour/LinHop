#ifndef LINHOP_MAINSCENE_HPP
#define LINHOP_MAINSCENE_HPP

#include <memory>

#include "spige.h"
#include "Utils.hpp"
#include "Lines.hpp"
#include "Ball.hpp"
#include "Tail.hpp"
#include "Sparks.hpp"
#include "Lasers.hpp"
#include "Label.hpp"

class MainScene : public Scene {
public:
    MainScene() = delete;
    explicit MainScene(Engine& e);

    ~MainScene() override;

    void suspend(Engine& engine) override;
    void resume(Engine& engine) override;
    void update(Engine& engine) override;
    void render(Engine& engine) override;

    struct audio audio_engine = {};
    struct audio_source audio_main = {};
    struct audio_source audio_alt = {};
    struct audio_source audio_bounce = {};
    struct audio_source audio_fail_a = {};
    struct audio_source audio_fail_b = {};
    struct audio_source audio_warning = {};

    // drawables
    struct line line = {};
    struct text small_text = {};
    struct text medium_text = {};
    struct text large_text = {};

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
    std::unique_ptr<Label> label_menu_hint;
    std::unique_ptr<Label> label_menu_mode;

    std::unique_ptr<Label> label_endgame_restart;
    std::unique_ptr<Label> label_endgame_score;

    std::unique_ptr<Label> label_settings_title;
    std::unique_ptr<Label> label_settings_fx;
    std::unique_ptr<Label> label_settings_music_volume;
    std::unique_ptr<Label> label_settings_unlock_resizing;
    std::unique_ptr<Label> label_settings_reset_statistics;
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
        FX_ENABLED, MUSIC_VOLUME, UNLOCK_RESIZE, RESET_STATISTICS, BACK, END
    } settings_selected = SettingsSelected::FX_ENABLED;

    void onEventPointerMove(Engine& engine);
    void onEventPointerDown();
    void onEventPointerUp(Engine& engine);
    void onEventSelect(Engine& engine);
    void onEventUp();
    void onEventLeft();
    void onEventDown();
    void onEventRight();
    void onEventBack(Engine& engine);

private:
    void reset(Engine& engine);

    glm::vec2 prev_mouse_pos {0};
    glm::vec2 last_click {0};
    Color background_color{0.0f, 0.1f, 0.2f, 1.f};

    struct SaveData {
        long maxScoreClassic;
        long maxScoreHidden;
        long fxEnabled;
        long unlockResizing;
        float musicVolumeFloat;
    } save_data {0, 0, 1, 0, .8f };

    static constexpr int ball_strength_mod = 25000;
    static constexpr int ball_gravity_mod = 8000;
    static constexpr float rand_lines_density = 350.0f; /* lower = higher */
    static constexpr float small_text_size = 28.f;
    static constexpr float medium_text_size = 64.f;
    static constexpr float large_text_size = 90.f;

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

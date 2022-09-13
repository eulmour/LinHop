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
    std::unique_ptr<Tail> ballTail, cursorTail;

    // labels
    std::unique_ptr<Label> labelMenuTitle;
    std::unique_ptr<Label> labelMenuContinue;
    std::unique_ptr<Label> labelMenuStart;
    std::unique_ptr<Label> labelMenuSettings;
    std::unique_ptr<Label> labelMenuExit;
    std::unique_ptr<Label> labelMenuHint;
    std::unique_ptr<Label> labelMenuMode;

    std::unique_ptr<Label> labelEndgameRestart;
    std::unique_ptr<Label> labelEndgameScore;

    std::unique_ptr<Label> labelSettingsTitle;
    std::unique_ptr<Label> labelSettingsFx;
    std::unique_ptr<Label> labelSettingsMusicVolume;
    std::unique_ptr<Label> labelSettingsUnlockResizing;
    std::unique_ptr<Label> labelSettingsResetStatistics;
    std::unique_ptr<Label> labelSettingsBack;

    std::unique_ptr<Label> labelGameScore;
    std::unique_ptr<Label> labelGameFps;

    enum class GameState {
        MENU, SETTINGS, INGAME, PAUSED, ENDGAME, EXITING, END
    } gameState = GameState::MENU;

    enum class GameMode {
        CLASSIC, HIDDEN, END
    } gameMode = GameMode::CLASSIC;

    enum class MenuSelected {
        CONTINUE, START, SETTINGS, EXIT, END
    } menuSelected = MenuSelected::START;

    enum class SettingsSelected {
        FX_ENABLED, MUSIC_VOLUME, UNLOCK_RESIZE, RESET_STATISTICS, BACK, END
    } settingsSelected = SettingsSelected::FX_ENABLED;

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

    glm::vec2 prevMousePos {0};
    glm::vec2 lastClick {0};
    Color backgroundColor{0.0f, 0.1f, 0.2f, 1.f};

    struct SaveData {
        long maxScoreClassic;
        long maxScoreHidden;
        long fxEnabled;
        long unlockResizing;
        float musicVolumeFloat;
    } saveData { 0, 0, 1, 0, .8f };

    static constexpr int ballStrengthMod = 25000;
    static constexpr int ballGravityMod = 8000;
    static constexpr float randLinesDensity = 350.0f; /* lower = higher */
    static constexpr float smallTextSize = 28.f;
    static constexpr float mediumTextSize = 50.f;
    static constexpr float largeTextSize = 72.f;

    float last_place = randLinesDensity;
    long gameScore = 0L;

protected:
    bool pressed {false};
    bool pressedOnce {false};
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

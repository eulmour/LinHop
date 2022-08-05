#ifndef LINHOP_MAINSCENE_HPP
#define LINHOP_MAINSCENE_HPP

#include "spige.h"
#include "Utils.hpp"
#include "Lines.hpp"
#include "Ball.hpp"
#include "Tail.hpp"
#include "Sparks.hpp"
#include "Lazers.hpp"
#include "Label.hpp"

class MainScene {
public:
    MainScene();
    ~MainScene();

    void pause();
    void resume();
    bool draw();
    void reset();

    vec4 backgroundColor{0.0f, 0.1f, 0.2f, 1.f};

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
    std::unique_ptr<Lazers> lazers;
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

    struct SaveData {
        long maxScoreClassic;
        long maxScoreHidden;
        long fxEnabled;
        long unlockResizing;
        float musicVolumeFloat;
    };

    enum class GameState
    {
        MENU, SETTINGS, INGAME, PAUSED, ENDGAME, EXITING, END
    } gameState = GameState::MENU;
//    } gameState = GameState::INGAME;

    enum class GameMode
    {
        CLASSIC, HIDDEN, END
    } gameMode = GameMode::CLASSIC;

    enum class MenuSelected
    {
        CONTINUE, START, SETTINGS, EXIT, END
    } menuSelected = MenuSelected::START;

    enum class SettingsSelected
    {
        FX_ENABLED, MUSIC_VOLUME, UNLOCK_RESIZE, RESET_STATISTICS, BACK, END
    } settingsSelected = SettingsSelected::FX_ENABLED;

    void onEventPointerDown();
    void onEventPointerUp();
    void onEventSelect();
    void onEventUp();
    void onEventLeft();
    void onEventDown();
    void onEventRight();
    bool onEventBack();

private:
    void update(float dt);

    SaveData saveData{
        .maxScoreClassic = 0,
        .maxScoreHidden = 0,
        .fxEnabled = 1,
        .unlockResizing = 0,
        .musicVolumeFloat = .8f
    };

    static constexpr int ballStrengthMod = 25000;
    static constexpr int ballGravityMod = 8000;
    static constexpr float randLinesDensity = 350.0f; /* lower = higher */
    static constexpr float smallTextSize = 34.f;
    static constexpr float mediumTextSize = 58.f;
    static constexpr float largeTextSize = 72.f;

    float last_place = randLinesDensity;
    bool pressed = false;
    bool tapped = false;
    vec2 prevMousePos = {0};
    vec2 lastClick = {0};
    long gameScore = 0L;
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

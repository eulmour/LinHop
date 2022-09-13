#ifndef LINHOP_SETTINGSSCENE_HPP
#define LINHOP_SETTINGSSCENE_HPP

#include "spige.h"
#include <memory>
#include "Utils.hpp"
#include "Label.hpp"

class SettingsScene : public Scene {

public:
    explicit SettingsScene(Engine& e);
    ~SettingsScene() override = default;
    void resume(Engine& engine) override;
    void suspend(Engine& engine) override;
    void update(Engine& engine) override;
    void render(Engine& engine) override;
    Color getBackgroundColor() const { return { 0.f, 0.f, 0.f, 1.f }; }
    void onEventPointerMove();
    void onEventPointerDown();
    void onEventPointerUp();
    void onEventSelect();
    void onEventUp();
    void onEventLeft();
    void onEventDown();
    void onEventRight();
    bool onEventBack();

private:
    static constexpr float smallTextSize = 28.f;
    static constexpr float mediumTextSize = 50.f;
    static constexpr float largeTextSize = 72.f;

    struct text small_text = {};
    struct text medium_text = {};
    struct text large_text = {};

    std::unique_ptr<Label> labelSettingsTitle;
    std::unique_ptr<Label> labelSettingsFx;
    std::unique_ptr<Label> labelSettingsMusicVolume;
    std::unique_ptr<Label> labelSettingsUnlockResizing;
    std::unique_ptr<Label> labelSettingsResetStatistics;
    std::unique_ptr<Label> labelSettingsBack;

    struct SaveData {
        long maxScoreClassic;
        long maxScoreHidden;
        long fxEnabled;
        long unlockResizing;
        float musicVolumeFloat;
    } saveData { 0, 0, 1, 0, .8f };

    enum class SettingsSelected {
        FX_ENABLED, MUSIC_VOLUME, UNLOCK_RESIZE, RESET_STATISTICS, BACK, END
    } settingsSelected = SettingsSelected::FX_ENABLED;

    static constexpr glm::vec4 COLOR_SELECTED { 0.6f, 0.9f, 1.0f, 1.f };
    static constexpr glm::vec4 COLOR_HIDDEN { 0.5f, 0.35f, 0.6f, 1.f };
    static constexpr glm::vec4 COLOR_IDLE { 0.4f, 0.55f, 0.6f, 1.f };
    static constexpr glm::vec4 COLOR_DISABLED { 0.2f, 0.35f, 0.4f, 1.f };
};

template<>
struct my_enum_is_unit_steppable<SettingsScene::SettingsSelected> {
    enum {
        value = true
    };
};
#endif //LINHOP_SETTINGSSCENE_HPP

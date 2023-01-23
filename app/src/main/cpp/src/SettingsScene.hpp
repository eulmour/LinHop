#ifndef LINHOP_SETTINGSSCENE_HPP
#define LINHOP_SETTINGSSCENE_HPP

#include "engine/engine.h"
#include <memory>
#include "Util.hpp"
#include "Label.hpp"

class SettingsScene : public wuh::Scene {

public:
    explicit SettingsScene(wuh::Engine& e);
    ~SettingsScene() override = default;
    void resume(wuh::Engine& engine) override;
    void suspend(wuh::Engine& engine) override;
    void update(wuh::Engine& engine);
    void render(wuh::Engine& engine) override;
    wuh::Color getBackgroundColor() const { return { 0.f, 0.f, 0.f, 1.f }; }
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
    static constexpr float small_text_size = 28.f;
    static constexpr float medium_text_size = 50.f;
    static constexpr float large_text_size = 72.f;

    std::unique_ptr<wuh::Text> small_text;
    std::unique_ptr<wuh::Text> medium_text;
    std::unique_ptr<wuh::Text> large_text;

    std::unique_ptr<Label> label_settings_title;
    std::unique_ptr<Label> label_settings_fx;
    std::unique_ptr<Label> label_settings_music_volume;
    std::unique_ptr<Label> label_settings_unlock_resizing;
    std::unique_ptr<Label> label_settings_reset_statistics;
    std::unique_ptr<Label> label_settings_back;

    struct SaveData {
        long max_score_classic;
        long max_score_hidden;
        long fx_enabled;
        long unlock_resizing;
        float music_volume_float;
    } save_data {0, 0, 1, 0, .8f };

    enum class SettingsSelected {
        FX_ENABLED, MUSIC_VOLUME, RESET_STATISTICS, BACK, END
    } settings_selected = SettingsSelected::FX_ENABLED;

    static constexpr wuh::Color COLOR_SELECTED { 0.6f, 0.9f, 1.0f, 1.f };
    static constexpr wuh::Color COLOR_HIDDEN { 0.5f, 0.35f, 0.6f, 1.f };
    static constexpr wuh::Color COLOR_IDLE { 0.4f, 0.55f, 0.6f, 1.f };
    static constexpr wuh::Color COLOR_DISABLED { 0.2f, 0.35f, 0.4f, 1.f };
};

template<>
struct my_enum_is_unit_steppable<SettingsScene::SettingsSelected> {
    enum {
        value = true
    };
};
#endif //LINHOP_SETTINGSSCENE_HPP

#include "SettingsScene.hpp"

#define PROLOG(e) \
    float screenW = static_cast<float>(e.window->getLogicalSize()[0]); \
    float screenH = static_cast<float>(e.window->getLogicalSize()[1]);

SettingsScene::SettingsScene(Engine& e) {

    PROLOG(e)

    this->label_settings_title = std::make_unique<Label>(&this->large_text, "Settings", glm::vec2 {
        screenW/2 - 170, screenH / 2.f - 280.f
    });

    this->label_settings_fx = std::make_unique<Label>(&this->medium_text, "FX: ", glm::vec2 {
        screenW/2 - 165, screenH / 2.f - 60.f
    });

    this->label_settings_music_volume = std::make_unique<Label>(&this->medium_text, "Volume: ", glm::vec2 {
        screenW/2 - 150, screenH / 2.f });

    this->label_settings_reset_statistics = std::make_unique<Label>(&this->medium_text, "Reset", glm::vec2 {
        screenW/2 - 76, screenH / 2.f + 120.f
    });

    this->label_settings_back = std::make_unique<Label>(&this->medium_text, "Back", glm::vec2 {
        screenW/2 - 58, screenH / 2.f + 280.f
    });

    struct file saveDataFile = {}; if (file_load(&saveDataFile, "savedata.dat")) {
        memcpy((void*)&this->save_data, saveDataFile.data, sizeof(SaveData));
        file_unload(&saveDataFile);
    }
}

void SettingsScene::resume(Engine&) {
    const char *const fontPath = "fonts/OCRAEXT.TTF";
    text_load(&this->small_text, fontPath, SettingsScene::small_text_size);
    text_load(&this->medium_text, fontPath, SettingsScene::medium_text_size);
    text_load(&this->large_text, fontPath, SettingsScene::large_text_size);
}

void SettingsScene::suspend(Engine&) {
    text_unload(&small_text);
    text_unload(&medium_text);
    text_unload(&large_text);
}

void SettingsScene::update(Engine&) {
}

void SettingsScene::render(Engine&) {

    this->label_settings_title->draw();

    this->label_settings_fx
        ->setText(CCAT("FX: ", save_data.fx_enabled, "enabled", "disabled"))
        .setColor(settings_selected == SettingsSelected::FX_ENABLED ? COLOR_SELECTED : COLOR_IDLE)
        .draw();

    this->label_settings_music_volume
        ->setText("Volume: " + std::to_string(static_cast<int>(save_data.music_volume_float * 100)))
        .setColor(settings_selected == SettingsSelected::MUSIC_VOLUME ? COLOR_SELECTED : COLOR_IDLE)
        .draw();

    this->label_settings_reset_statistics
        ->setColor(settings_selected == SettingsSelected::RESET_STATISTICS ? COLOR_SELECTED : COLOR_IDLE)
        .draw();

    this->label_settings_back
        ->setColor(settings_selected == SettingsSelected::BACK ? COLOR_SELECTED : COLOR_IDLE)
        .draw();
}

void SettingsScene::onEventPointerMove() {

}

void SettingsScene::onEventPointerDown() {
}

void SettingsScene::onEventPointerUp() {
}

void SettingsScene::onEventSelect() {
    switch (settings_selected) {
        case SettingsSelected::FX_ENABLED:
            save_data.fx_enabled = save_data.fx_enabled == 0;
            break; /* end of FX_ENABLED */

        case SettingsSelected::RESET_STATISTICS:

            save_data.max_score_classic = 0L;
            save_data.max_score_hidden = 0L;
            file_remove("savedata.dat");

            break; /* end of RESET_STATISTICS */

        case SettingsSelected::BACK:
            // TODO implement switching scenes
            break; /* end of BACK */
        default:
            break;
    }

}
void SettingsScene::onEventUp() {
    --settings_selected;
}
void SettingsScene::onEventLeft() {}
void SettingsScene::onEventDown() {
    ++settings_selected;
}
void SettingsScene::onEventRight() {}
bool SettingsScene::onEventBack() { return false; }
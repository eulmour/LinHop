#include "SettingsScene.hpp"

extern "C" spige *spige_instance;

#define CX (static_cast<float>(spige_instance->width) / 2.f) /* center x */
#define CY (static_cast<float>(spige_instance->height) / 2.f) /* center y */

SettingsScene::SettingsScene() {
    this->labelSettingsTitle = std::make_unique<Label>(&this->large_text, "Settings", glm::vec2 {
            CX - 170, static_cast<float>(spige_instance->height) / 2.f - 280.f
    });

    this->labelSettingsFx = std::make_unique<Label>(&this->medium_text, "FX: ", glm::vec2 {
            CX - 165, static_cast<float>(spige_instance->height) / 2.f - 60.f
    });

    this->labelSettingsMusicVolume = std::make_unique<Label>(&this->medium_text, "Volume: ", glm::vec2 {
            CX - 150, static_cast<float>(spige_instance->height) / 2.f
    });

    this->labelSettingsUnlockResizing = std::make_unique<Label>(&this->medium_text, "Resizing: ", glm::vec2 {
            CX - 182, static_cast<float>(spige_instance->height) / 2.f + 60.f
    });

#if defined(ANDROID)
    this->labelSettingsUnlockResizing->setColor(COLOR_DISABLED);
#endif

    this->labelSettingsResetStatistics = std::make_unique<Label>(&this->medium_text, "Reset", glm::vec2 {
            CX - 76, static_cast<float>(spige_instance->height) / 2.f + 120.f
    });

    this->labelSettingsBack = std::make_unique<Label>(&this->medium_text, "Back", glm::vec2 {
            CX - 58, static_cast<float>(spige_instance->height) / 2.f + 280.f
    });

    struct file saveDataFile = {}; if (file_load(&saveDataFile, "savedata.dat")) {
        memcpy((void*)&this->saveData, saveDataFile.data, sizeof(SaveData));
        file_unload(&saveDataFile);
    }
}

SettingsScene::~SettingsScene() {

}

void SettingsScene::resume(Engine& engine) {
    const char *const fontPath = "fonts/OCRAEXT.TTF";
    text_load(&this->small_text, fontPath, SettingsScene::smallTextSize);
    text_load(&this->medium_text, fontPath, SettingsScene::mediumTextSize);
    text_load(&this->large_text, fontPath, SettingsScene::largeTextSize);
}

void SettingsScene::suspend(Engine& engine) {
    text_unload(&small_text);
    text_unload(&medium_text);
    text_unload(&large_text);
}

bool SettingsScene::update(Engine& engine) {
    return true;
}

void SettingsScene::render(Engine& engine) {

    this->labelSettingsTitle->draw();

    this->labelSettingsFx
        ->setText(CCAT("FX: ", saveData.fxEnabled, "enabled", "disabled"))
        .setColor(settingsSelected == SettingsSelected::FX_ENABLED ? COLOR_SELECTED : COLOR_IDLE)
        .draw();

    this->labelSettingsMusicVolume
        ->setText("Volume: " + std::to_string(static_cast<int>(saveData.musicVolumeFloat * 100)))
        .setColor(settingsSelected == SettingsSelected::MUSIC_VOLUME ? COLOR_SELECTED : COLOR_IDLE)
        .draw();

    this->labelSettingsUnlockResizing
        ->setText(CCAT("Resizing: ", saveData.unlockResizing, "yes", "no"))
        .setColor(settingsSelected == SettingsSelected::UNLOCK_RESIZE ? COLOR_SELECTED : COLOR_IDLE)
        .draw();

    this->labelSettingsResetStatistics
        ->setColor(settingsSelected == SettingsSelected::RESET_STATISTICS ? COLOR_SELECTED : COLOR_IDLE)
        .draw();

    this->labelSettingsBack
        ->setColor(settingsSelected == SettingsSelected::BACK ? COLOR_SELECTED : COLOR_IDLE)
        .draw();
}

void SettingsScene::onEventPointerMove() {

}

void SettingsScene::onEventPointerDown() {
}

void SettingsScene::onEventPointerUp() {
}

void SettingsScene::onEventSelect() {
    switch (settingsSelected) {
        case SettingsSelected::FX_ENABLED:
            saveData.fxEnabled = saveData.fxEnabled == 0;
            break; /* end of FX_ENABLED */

        case SettingsSelected::UNLOCK_RESIZE: {
            if (saveData.unlockResizing == 1L) {
                /* little evil */
                saveData.maxScoreClassic = 0L;
                saveData.maxScoreHidden = 0L;
            }

            saveData.unlockResizing = saveData.unlockResizing == 0L ? 1L : 0L;
//                    SaveGameData(saveData);
//                    Quit();
            break; /* end of UNLOCK_RESIZE */
        }

        case SettingsSelected::RESET_STATISTICS:

            saveData.maxScoreClassic = 0L;
            saveData.maxScoreHidden = 0L;
            file_remove("savedata.dat");

            break; /* end of RESET_STATISTICS */

        case SettingsSelected::BACK:

//            gameState = GameState::PAUSED;

            break; /* end of BACK */
        default:
            break;
    }

}
void SettingsScene::onEventUp() {
    --settingsSelected;
}
void SettingsScene::onEventLeft() {}
void SettingsScene::onEventDown() {
    ++settingsSelected;
}
void SettingsScene::onEventRight() {}
bool SettingsScene::onEventBack() { return false; }
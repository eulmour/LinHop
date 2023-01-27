#include "SettingsActivity.hpp"

// #define PROLOG(e) \
    // float screenW = static_cast<float>(e.window->physical_size()[0]); \
    // float screenH = static_cast<float>(e.window->physical_size()[1]);

#define PROLOG(e) \
    float screenW = static_cast<float>(e.window->size()[0]); \
    float screenH = static_cast<float>(e.window->size()[1]);

SettingsActivity::SettingsActivity(wuh::Engine& e) {

    PROLOG(e)

    this->label_settings_title = std::make_unique<Label>("Settings", glm::vec2 {
        screenW/2 - 170, screenH / 2.f - 280.f
    });

    this->label_settings_fx = std::make_unique<Label>("FX: ", glm::vec2 {
        screenW/2 - 165, screenH / 2.f - 60.f
    });

    this->label_settings_music_volume = std::make_unique<Label>("Volume: ", glm::vec2 {
        screenW/2 - 150, screenH / 2.f });

    this->label_settings_reset_statistics = std::make_unique<Label>("Reset", glm::vec2 {
        screenW/2 - 76, screenH / 2.f + 120.f
    });

    this->label_settings_back = std::make_unique<Label>("Back", glm::vec2 {
        screenW/2 - 58, screenH / 2.f + 280.f
    });

    try {
        wuh::File save_data_file("savedata.dat");
        memcpy((void*)&this->save_data, save_data_file.data(), sizeof(SaveData));
    } catch (const std::exception& exception) {
        e.log() << exception.what() << "\n";
        e.show_log();
    }
}

void SettingsActivity::resume(wuh::Engine&) {
    this->small_text = std::make_unique<wuh::Text>(nullptr, SettingsActivity::small_text_size);
    this->medium_text = std::make_unique<wuh::Text>(nullptr, SettingsActivity::medium_text_size);
    this->large_text = std::make_unique<wuh::Text>(nullptr, SettingsActivity::large_text_size);
}

void SettingsActivity::suspend(wuh::Engine&) {
    this->small_text.reset();
    this->medium_text.reset();
    this->large_text.reset();
}

void SettingsActivity::update(wuh::Engine&) {
}

void SettingsActivity::render(wuh::Engine& e) {

    this->update(e);

    this->label_settings_title->draw(e.graphics, *this->large_text);

    this->label_settings_fx
        ->setText(CCAT("FX: ", save_data.fx_enabled, "enabled", "disabled"))
        .setColor(settings_selected == SettingsSelected::FX_ENABLED ? COLOR_SELECTED : COLOR_IDLE)
        .draw(e.graphics, *this->medium_text);

    this->label_settings_music_volume
        ->setText("Volume: " + std::to_string(static_cast<int>(save_data.music_volume_float * 100)))
        .setColor(settings_selected == SettingsSelected::MUSIC_VOLUME ? COLOR_SELECTED : COLOR_IDLE)
        .draw(e.graphics, *this->medium_text);

    this->label_settings_reset_statistics
        ->setColor(settings_selected == SettingsSelected::RESET_STATISTICS ? COLOR_SELECTED : COLOR_IDLE)
        .draw(e.graphics, *this->medium_text);

    this->label_settings_back
        ->setColor(settings_selected == SettingsSelected::BACK ? COLOR_SELECTED : COLOR_IDLE)
        .draw(e.graphics, *this->medium_text);
}

void SettingsActivity::onEventPointerMove() {

}

void SettingsActivity::onEventPointerDown() {
}

void SettingsActivity::onEventPointerUp() {
}

void SettingsActivity::onEventSelect() {
    switch (settings_selected) {
        case SettingsSelected::FX_ENABLED:
            save_data.fx_enabled = save_data.fx_enabled == 0;
            break; // end of FX_ENABLED

        case SettingsSelected::RESET_STATISTICS:

            save_data.max_score_classic = 0L;
            save_data.max_score_hidden = 0L;
            // wuh::file_remove("savedata.dat");
            wuh::File::remove("savedata.dat");

            break; // end of RESET_STATISTICS

        case SettingsSelected::BACK:
            // TODO implement switching scenes
            break; // end of BACK
        default:
            break;
    }

}
void SettingsActivity::onEventUp() {
    --settings_selected;
}
void SettingsActivity::onEventLeft() {}
void SettingsActivity::onEventDown() {
    ++settings_selected;
}
void SettingsActivity::onEventRight() {}
bool SettingsActivity::onEventBack() { return false; }
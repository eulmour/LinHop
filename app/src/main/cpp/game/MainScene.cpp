#include "MainScene.hpp"
#include <memory>

extern "C" spige *spige_instance;

#define CX (static_cast<float>(spige_instance->width) / 2) /* center x */

/* colors --- I used macros because cglm don't deal with consts */
#define COLOR_SELECTED glm::vec4{ 0.6f, 0.9f, 1.0f, 1.f }
#define COLOR_HIDDEN glm::vec4{ 0.5f, 0.35f, 0.6f, 1.f }
#define COLOR_IDLE glm::vec4{ 0.4f, 0.55f, 0.6f, 1.f }
#define COLOR_DISABLED glm::vec4{ 0.2f, 0.35f, 0.4f, 1.f }

// global
float scroll = 0.f;

MainScene::MainScene() {

    srand((unsigned)time(nullptr));

    int ret = 0;
    ret += audio_source_load(&audio_main, "audio/a.wav", .5f);
    ret += audio_source_load(&audio_alt, "audio/b.wav", .5f);
    ret += audio_source_load(&audio_bounce, "audio/bounce.wav", 1.f);
    ret += audio_source_load(&audio_fail_a, "audio/fail.wav", 1.f);
    ret += audio_source_load(&audio_fail_b, "audio/fail2.wav", 1.f);
    ret += audio_source_load(&audio_warning, "audio/warning.wav", 1.f);

    if (ret < 6)
        LOGE("Failed to load audio sources\n");

    struct file saveDataFile = {}; if (file_load(&saveDataFile, "savedata.dat")) {
        memcpy((void*)&this->saveData, saveDataFile.data, sizeof(SaveData));
        file_unload(&saveDataFile);
    }

    glm_vec2_copy(&glm::vec2 {
        static_cast<float>(spige_instance->width) / 2.f,
        static_cast<float>(spige_instance->height)
    }[0], ::spige_instance->cursor[0]);
    
    this->prevMousePos = {::spige_instance->cursor[0][0], ::spige_instance->cursor[0][1]};

    // glm_vec2_copy(::spige_instance->cursor[0], this->prevMousePos);

    // class
    lines       = std::make_unique<Lines>(&line);
    rand_lines  = std::make_unique<Lines>(&line);
    sparks      = std::make_unique<Sparks>();
    lazers      = std::make_unique<Lazers>(&line);
    ballTail    = std::make_unique<Tail>(&line, .7f);
    cursorTail  = std::make_unique<Tail>(&line, .08f);
    ball        = std::make_unique<Ball>();

    lines->Reset();

    // labels
    this->labelMenuTitle = std::make_unique<Label>(&this->large_text, "LinHop", glm::vec2 {
        CX - 118, static_cast<float>(spige_instance->height) / 2.f - 180.f
    });
    this->labelMenuTitle->setColor(glm::vec4 {0.6f, 0.8f, 1.0f, 1.f});

    this->labelMenuContinue = std::make_unique<Label>(&this->medium_text, "Continue", glm::vec2 {
        CX - 134, static_cast<float>(spige_instance->height) / 2.f - 40.f
    });

    this->labelMenuStart = std::make_unique<Label>(&this->medium_text, "Start", glm::vec2 {
        CX - 80, static_cast<float>(spige_instance->height) / 2.f + 20.f
    });

    this->labelMenuSettings = std::make_unique<Label>(&this->medium_text, "Settings", glm::vec2 {
        CX - 134, static_cast<float>(spige_instance->height) / 2.f + 80.f
    });

    this->labelMenuExit = std::make_unique<Label>(&this->medium_text, "Exit", glm::vec2 {
        CX - 68, static_cast<float>(spige_instance->height) / 2.f + 140.f
    });

    this->labelMenuHint = std::make_unique<Label>(&this->small_text, "Left or right to change mode", glm::vec2 {
        CX - 300, static_cast<float>(spige_instance->height - 40)
    });
    this->labelMenuHint->setColor(glm::vec4 {0.4f, 0.55f, 0.6f, 1.f});

    this->labelMenuMode = std::make_unique<Label>(&this->small_text, "Classic", glm::vec2 {
        CX - 74, static_cast<float>(spige_instance->height) / 2.f - 110.f
    });

    this->labelEndgameRestart = std::make_unique<Label>(&this->medium_text, "Press R", glm::vec2 {
        CX + 29, static_cast<float>(spige_instance->height) / 2.f
    });

    this->labelEndgameScore = std::make_unique<Label>(&this->medium_text, "Score: ", glm::vec2 {
        CX - 273, static_cast<float>(spige_instance->height) / 2.f - 60.f
    });

    this->labelSettingsTitle = std::make_unique<Label>(&this->large_text, "Settings", glm::vec2 {
        CX - 165, static_cast<float>(spige_instance->height) / 2.f - 180.f
    });

    this->labelSettingsFx = std::make_unique<Label>(&this->medium_text, "FX: ", glm::vec2 {
        CX - 190, static_cast<float>(spige_instance->height) / 2.f - 60.f
    });

    this->labelSettingsMusicVolume = std::make_unique<Label>(&this->medium_text, "Music volume: ", glm::vec2 {
        CX - 280, static_cast<float>(spige_instance->height) / 2.f
    });

    this->labelSettingsUnlockResizing = std::make_unique<Label>(&this->medium_text, "Unlock resizing: ", glm::vec2 {
        CX - 332, static_cast<float>(spige_instance->height) / 2.f + 60.f
    });

#if defined(ANDROID)
    this->labelSettingsUnlockResizing->setColor(COLOR_DISABLED);
#endif

    this->labelSettingsResetStatistics = std::make_unique<Label>(&this->medium_text, "Reset statistics", glm::vec2 {
        CX - 270, static_cast<float>(spige_instance->height) / 2.f + 120.f
    });

    this->labelSettingsBack = std::make_unique<Label>(&this->medium_text, "Back", glm::vec2 {
        CX - 62, static_cast<float>(spige_instance->height) / 2.f + 180.f
    });

    this->labelGameScore = std::make_unique<Label>(&this->medium_text, "Score: ", glm::vec2 {
        0.0f, MainScene::mediumTextSize
    });

    this->labelGameFps = std::make_unique<Label>(&this->medium_text, " fps", glm::vec2 {
        static_cast<float>(spige_instance->width) - 80.0f, MainScene::mediumTextSize
    });

    /* init clicks */
    lastClick[0] = static_cast<float>(spige_instance->width) / 2.f;
    lastClick[1] = static_cast<float>(spige_instance->height);

    if (!audio_init(&audio_engine))
        LOGE("Failed to initialize audio engine.\n");
}

MainScene::~MainScene() {

    file_save("savedata.dat", (void*)&this->saveData, sizeof(SaveData));

    audio_destroy(&this->audio_engine);
    audio_source_unload(&this->audio_main);
    audio_source_unload(&this->audio_alt);
    audio_source_unload(&this->audio_bounce);
    audio_source_unload(&this->audio_fail_a);
    audio_source_unload(&this->audio_fail_b);
    audio_source_unload(&this->audio_warning);
}

void MainScene::pause() {

    this->ball->deactivate();
    this->lazers->deactivate();
    this->sparks->deactivate();

    line_unload(&line);

    text_unload(&small_text);
    text_unload(&medium_text);
    text_unload(&large_text);

    audio_pause_all(&audio_engine);
}

void MainScene::resume() {

    line_load(&this->line);
    line.width = 5.f;

    const char *const fontPath = "fonts/OCRAEXT.TTF";
    text_load(&this->small_text, fontPath, MainScene::smallTextSize);
    text_load(&this->medium_text, fontPath, MainScene::mediumTextSize);
    text_load(&this->large_text, fontPath, MainScene::largeTextSize);

    this->ball->activate();
    this->lazers->activate();
    this->sparks->activate();

    audio_play_all(&audio_engine);
}

void MainScene::update(float dt) {

    switch (this->gameState) {
        case GameState::MENU:
        case GameState::ENDGAME:
        case GameState::INGAME:

            gameScore = std::max(gameScore, -static_cast<long>(
                ball->pos[1] - static_cast<float>(spige_instance->height) / 2.f));

            ball->bounceStrength = 1 + static_cast<float>(gameScore) / ballStrengthMod;
            ball->gravity = 9.8f + static_cast<float>(gameScore) / ballGravityMod;

            ball->Move(dt);

            if (ball->Collision(*lines, ball->prev_pos)) {
                sparks->Push(ball->pos);
                audio_play(&this->audio_engine, &this->audio_bounce);
            }

            if (ball->Collision(*rand_lines, ball->prev_pos)) {
                sparks->Push(ball->pos);
                audio_play(&this->audio_engine, &this->audio_bounce);
            }

            /* If ball reaches half of the screen then update scroll */
            if (ball->pos[1] - static_cast<float>(spige_instance->height) / 2.f - 10.f < scroll) {
                scroll += (ball->pos[1] - static_cast<float>(spige_instance->height) / 2.f - 10.f - scroll) / 10.f;
            }

            /* If game was over turn global scroll back */
            if (gameState == GameState::ENDGAME) {
                scroll += (-scroll) / 100;
            }

            /* If the ball is out of screen then stop the game */
            if (ball->pos[0] < 0 || ball->pos[0] > static_cast<float>(spige_instance->width) ||
                ball->pos[1] - scroll > static_cast<float>(spige_instance->height) + ball->radius)
            {
                if (gameState == GameState::INGAME) {
                    if (t_rand(0, 1) == 0)
                        audio_play(&this->audio_engine, &this->audio_fail_a);
                    else
                        audio_play(&this->audio_engine, &this->audio_fail_b);

                    gameState = GameState::ENDGAME;
                }
            }

            /* Random platforms */
            if ((-scroll) - last_place > randLinesDensity) {
                if (t_rand(0, 1) <= 1) {
                    auto base_y = scroll - 80.0f;
                    auto base_x = t_rand(-((float)spige_instance->width/3), (float)spige_instance->width);

                    struct line {
                        glm::vec2 first;
                        glm::vec2 second;
                    } new_line {
                        {base_x, base_y},
                        {base_x + (static_cast<float>(t_rand(0, spige_instance->width)) / 2.f) - CX / 4,
							base_y + static_cast<float>(t_rand(0, spige_instance->height)) / 6.f}
                    };

                    if (dis_func(new_line.second[0] - new_line.first[0],
                                 new_line.second[1] - new_line.first[1]) > 30.0f) {
                        rand_lines->Push(new_line.second, new_line.first, false);
                    }
                }

                last_place += randLinesDensity;
            }

            /* Push for tail */
            if (this->saveData.fxEnabled) {

                ballTail->Push(glm::vec2 {
                        ball->pos[0] + ball->diameter,
                        ball->pos[1] + ball->diameter
                    }, glm::vec2{
                        ball->prev_pos[0] + ball->diameter,
                        ball->prev_pos[1] + ball->diameter
                    }
                );

                if (pressed) {
                    cursorTail->Push(
                        glm::vec2 {::spige_instance->cursor[0][0],
                                ::spige_instance->cursor[0][1] + scroll},
                        prevMousePos);
                }
            }

            /* lazers */
            if (gameScore > 1000L) {
                if (t_rand(0, 600) == 1) {
                    audio_play(&this->audio_engine, &this->audio_warning);

                    lazers->Trigger(
                        t_rand(0.0f, static_cast<float>(spige_instance->width) - LAZERS_WIDTH));
                }

                if (lazers->liveTime == 59)
                    audio_play(&this->audio_engine, &this->audio_warning);
            }

            break;

        default:
            break;
    }

    /* gestures */
    switch (this->gameState) {

        case GameState::MENU:
        case GameState::PAUSED:
            if (this->labelMenuContinue->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {
                if (this->tapped) {
                    this->menuSelected = MenuSelected::CONTINUE;
                    this->onEventSelect();
                }
            }

            if (this->labelMenuStart->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {
                if (this->tapped) {
                    this->menuSelected = MenuSelected::START;
                    this->onEventSelect();
                }
            }
            if (this->labelMenuSettings->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {
                if (this->tapped) {
                    this->menuSelected = MenuSelected::SETTINGS;
                    this->onEventSelect();
                }
            }

            if (this->labelMenuExit->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {
                if (this->tapped) {
                    this->menuSelected = MenuSelected::EXIT;
                    this->onEventSelect();
                }
            }

            if (this->labelMenuHint->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {
                if (this->tapped) {
                    this->labelMenuHint->setColor(glm::vec4 {
                        t_rand(.0f, 1.f), t_rand(.0f, 1.f), t_rand(.0f, 1.f), 1.f
                    });
                }
            }

            if (this->labelMenuMode->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {
                if (this->tapped) {
                    this->onEventRight();
                }
            }

            break;

        case GameState::ENDGAME:

            if (this->labelEndgameRestart->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {
                if (this->tapped)
                    this->onEventSelect();
            }
            break;

        case GameState::SETTINGS:

            if (this->labelSettingsFx->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {

                if (this->tapped) {
                    this->settingsSelected = SettingsSelected::FX_ENABLED;
                    this->onEventSelect();
                }
            }

            if (this->labelSettingsMusicVolume->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {

                if (this->tapped) {
                    this->settingsSelected = SettingsSelected::MUSIC_VOLUME;
                    this->onEventLeft();
                }
            }

            if (this->labelSettingsUnlockResizing->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {

                if (this->tapped) {
                    this->settingsSelected = SettingsSelected::UNLOCK_RESIZE;
                    this->onEventSelect();
                }
            }

            if (this->labelSettingsResetStatistics->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {

                if (this->tapped) {
                    this->settingsSelected = SettingsSelected::RESET_STATISTICS;
                    this->onEventSelect();
                }
            }

            if (this->labelSettingsBack->isCollide(glm::make_vec2(::spige_instance->cursor[0]))) {

                if (this->tapped) {
                    this->settingsSelected = SettingsSelected::BACK;
                    this->onEventSelect();
                }
            }
            break;
        default: break;
    }

    /* play random music */
    if (this->audio_main.state != STATE_BUSY
        && this->audio_alt.state != STATE_BUSY)
    {
        if (rand() % 2 == 0)
            audio_play(&audio_engine, &audio_main);
        else
            audio_play(&audio_engine, &audio_alt);
    }
}

bool MainScene::draw() {

    static float dt = 1.f / 60.f;

    update(dt);

    // update background color
    static float bgColorDirection = 0.005f * dt;
    if (backgroundColor[0] > 0.2f || backgroundColor[0] < 0.0f)
        bgColorDirection = -bgColorDirection;

    backgroundColor[0] += -bgColorDirection / 2;
    backgroundColor[1] += bgColorDirection / 3;
    backgroundColor[2] += bgColorDirection / 2;

    switch (gameState) {
        case GameState::PAUSED:
        case GameState::MENU:

            this->labelMenuTitle->draw();

            this->labelMenuContinue
                ->setColor(menuSelected == MenuSelected::CONTINUE ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            this->labelMenuStart
                ->setColor(menuSelected == MenuSelected::START ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            this->labelMenuSettings
                ->setColor(menuSelected == MenuSelected::SETTINGS ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            this->labelMenuExit
                ->setColor(menuSelected == MenuSelected::EXIT ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            this->labelMenuHint->draw();

            switch (gameMode) {
                case GameMode::CLASSIC:

                    this->labelGameScore
                        ->setText("High score: " + std::to_string(saveData.maxScoreClassic))
                        .setColor(COLOR_IDLE)
                        .draw();

                    this->labelMenuMode
                        ->setText("Classic")
                        .setPos(glm::vec2{ CX - 74, static_cast<float>(spige_instance->height) / 2.f - 110.f })
                        .setColor(COLOR_IDLE)
                        .draw();

                    break;

                case GameMode::HIDDEN:

                    this->labelGameScore
                        ->setText("High score: " + std::to_string(saveData.maxScoreClassic))
                        .setColor(COLOR_IDLE)
                        .draw();

                    this->labelMenuMode
                        ->setText("Hidden")
                        .setPos(glm::vec2{ CX - 70, static_cast<float>(spige_instance->height) / 2.f - 110.f })
                        .setColor(COLOR_IDLE)
                        .draw();

                    break;

                default:
                    break;
            }

            break;

        case GameState::SETTINGS:

            this->labelSettingsTitle->draw();

            this->labelSettingsFx
                ->setText(CCAT("FX: ", saveData.fxEnabled, "enabled", "disabled"))
                .setColor(settingsSelected == SettingsSelected::FX_ENABLED ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            this->labelSettingsMusicVolume
                ->setText("Music volume: " + std::to_string(static_cast<int>(saveData.musicVolumeFloat * 100)))
                .setColor(settingsSelected == SettingsSelected::MUSIC_VOLUME ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            this->labelSettingsUnlockResizing
                ->setText(CCAT("Unlock resizing: ", saveData.unlockResizing, "yes", "no"))
                .setColor(settingsSelected == SettingsSelected::UNLOCK_RESIZE ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            this->labelSettingsResetStatistics
                ->setColor(settingsSelected == SettingsSelected::RESET_STATISTICS ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            this->labelSettingsBack
                ->setColor(settingsSelected == SettingsSelected::BACK ? COLOR_SELECTED : COLOR_IDLE)
                .draw();

            break;

        case GameState::INGAME:

            if (pressed && gameMode == GameMode::CLASSIC) {
                glm_vec4_copy(&glm::vec4{0.5f, 0.5f, 0.5f, 1.0f}[0], this->line.color);
                line_draw(&this->line, &glm::vec4 {lastClick[0], lastClick[1] - scroll,
                                               ::spige_instance->cursor[0][0],
                                               ::spige_instance->cursor[0][1]}[0]);
            }

#ifdef DEBUG
            glm_vec4_copy(gameMode == GameMode::CLASSIC ? &COLOR_SELECTED[0] : &COLOR_HIDDEN[0], this->medium_text.color);

            this->labelGameFps
                ->setText(std::to_string(static_cast<int>(1 / dt)) + std::string(" fps"))
                .draw();
#endif

            this->labelGameScore
                ->setText("Score: " + std::to_string(gameScore))
                .setColor(gameMode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN)
                .draw();

            break;

        case GameState::ENDGAME:

            this->labelEndgameScore
                ->setText("Score: " + std::to_string(gameScore))
                .setColor(gameMode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN)
                .draw();

            this->labelEndgameRestart->draw();

            break;

        default:
            break;
    }

    lazers->Draw();

    if (lazers->liveTime == 0 && !lazers->lazers.empty()) {
        // lazer will destroy ball

        if (ball->pos[0] > lazers->lazers.front().a[0] &&
            ball->pos[0] < lazers->lazers.back().a[0]) {

            if (gameState == GameState::INGAME)
                gameState = GameState::ENDGAME;

            if (t_rand(0, 1) == 0)
                audio_play(&this->audio_engine, &this->audio_fail_a);
            else
                audio_play(&this->audio_engine, &this->audio_fail_b);
        }

        sparks->Push(glm::vec2 {0.0f, 0.0f});

        for (int i = 0; i < spige_instance->height; i += spige_instance->height / 10) {
            sparks->Push(glm::vec2 {lazers->lazers.back().a[0], static_cast<float>(i)});
            sparks->Push(glm::vec2 {lazers->lazers.front().a[0], static_cast<float>(i)});
        }
    }

    ball->Draw();

    if (gameMode == GameMode::CLASSIC)
        lines->Draw();

    if (saveData.fxEnabled) {
        cursorTail->Draw();
        sparks->Draw();
        ballTail->Draw();
    }

    rand_lines->Draw();

    this->prevMousePos = {
        ::spige_instance->cursor[0][0],
        ::spige_instance->cursor[0][1] + scroll,
    };

    this->tapped = false;
    return this->gameState != GameState::EXITING;
}

void MainScene::reset() {

    cursorTail->Reset();

    this->prevMousePos = {
        static_cast<float>(spige_instance->width) / 2.f,
        static_cast<float>(spige_instance->height)
    };

    this->lastClick = this->prevMousePos;

    scroll = 0.0f;

    if (gameMode == GameMode::CLASSIC)
        if (gameScore > saveData.maxScoreClassic)
            saveData.maxScoreClassic = gameScore;

    if (gameMode == GameMode::HIDDEN)
        if (gameScore > saveData.maxScoreHidden)
            saveData.maxScoreHidden = gameScore;

    gameScore = 0L;

    gameState = GameState::INGAME;
    last_place = randLinesDensity;

    ball->Reset();
    rand_lines->Reset();
    lines->Reset();
}

void MainScene::onEventPointerDown() {
    pressed = true;
}

void MainScene::onEventPointerUp() {

    if (this->gameState == GameState::INGAME) {
        glm::vec2 newPos = {
            ::spige_instance->cursor[0][0], ::spige_instance->cursor[0][1] + scroll
        };
        
        this->lines->Push(newPos, lastClick);
        this->lastClick = newPos;
    }

    this->pressed = false;
    this->tapped = true;
}

void MainScene::onEventSelect() {

    switch (gameState) {
        case GameState::SETTINGS:

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

                    gameState = GameState::PAUSED;

                    break; /* end of BACK */
                default:
                    break;
            }

            break;

        case GameState::PAUSED:
        case GameState::MENU:

            switch (menuSelected) {
                case MenuSelected::START:
                    gameState = GameState::INGAME;
                    reset();
                    break;

                case MenuSelected::CONTINUE:
                    if (gameState == GameState::PAUSED)
                        gameState = GameState::INGAME;
                    break;

                case MenuSelected::SETTINGS:
                    gameState = GameState::SETTINGS;
                    break;

                case MenuSelected::EXIT: {
                    this->gameState = GameState::EXITING;
                    break;
                }

                default:
                    break;
            }

            break;

        case GameState::ENDGAME:
            reset();
            break;
        default:
            return;
    }
}

bool MainScene::onEventBack() {
    switch (gameState) {

        case GameState::MENU:
            return false;

        case GameState::SETTINGS:
            gameState = GameState::PAUSED;
            break;

        case GameState::INGAME:
            menuSelected = MenuSelected::CONTINUE;
            gameState = GameState::PAUSED;
            break;

        case GameState::PAUSED:
            gameState = GameState::INGAME;
            break;

        case GameState::ENDGAME:
            gameState = GameState::MENU;
            break;

        default:
            break;
    }

    return true;
}

void MainScene::onEventUp() {

    if (gameState == GameState::MENU)
        --menuSelected;
    if (gameState == GameState::PAUSED)
        --menuSelected;
    if (gameState == GameState::SETTINGS)
        --settingsSelected;
}

void MainScene::onEventLeft() {

    if (gameState == GameState::MENU) {
        --gameMode;
        cursorTail->alpha = cursorTail->alpha == 0.15f ? 0.4f : 0.15f;
    }

    if (gameState == GameState::SETTINGS) {
        if (settingsSelected == SettingsSelected::MUSIC_VOLUME) {

            if (saveData.musicVolumeFloat > 0.0f) {
                saveData.musicVolumeFloat -= 0.1f;
            } else {
                saveData.musicVolumeFloat = 1.f;
            }

            this->audio_engine.master_vol = std::min(saveData.musicVolumeFloat, 1.f);
        }
    }
}

void MainScene::onEventDown() {

    if (gameState == GameState::MENU)
        ++menuSelected;
    if (gameState == GameState::PAUSED)
        ++menuSelected;
    if (gameState == GameState::SETTINGS)
        ++settingsSelected;
}

void MainScene::onEventRight() {

    if (gameState == GameState::MENU) {
        ++gameMode;
        cursorTail->alpha = cursorTail->alpha == 0.08f ? 0.4f : 0.08f;
    }

    if (gameState == GameState::SETTINGS) {

        if (settingsSelected == SettingsSelected::MUSIC_VOLUME) {

            if (saveData.musicVolumeFloat <= 1.0f) {
                saveData.musicVolumeFloat += 0.1f;
            } else {
                saveData.musicVolumeFloat = 0.f;
            }

            this->audio_engine.master_vol = std::min(saveData.musicVolumeFloat, 1.f);
        }
    }
}

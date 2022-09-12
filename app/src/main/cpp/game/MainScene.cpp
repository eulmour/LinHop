#include "MainScene.hpp"
#include <memory>

//extern "C" spige *spige_instance;

#define CX (static_cast<float>(spige_instance->width) / 2.f) /* center x */
#define CY (static_cast<float>(spige_instance->height) / 2.f) /* center y */

/* colors --- I used macros because cglm don't deal with consts */
#define COLOR_SELECTED glm::vec4{ 0.6f, 0.9f, 1.0f, 1.f }
#define COLOR_HIDDEN glm::vec4{ 0.5f, 0.35f, 0.6f, 1.f }
#define COLOR_IDLE glm::vec4{ 0.4f, 0.55f, 0.6f, 1.f }
#define COLOR_DISABLED glm::vec4{ 0.2f, 0.35f, 0.4f, 1.f }

#define PROLOG(e) float& pointerX = e.input.getPointerArray()[0][0]; \
    float& pointerY = e.input.getPointerArray()[0][1]; \
    float screenW = static_cast<float>(e.window->getLogicalSize()[0]); \
    float screenH = static_cast<float>(e.window->getLogicalSize()[1]);

// global
float scroll = 0.f;

MainScene::MainScene(Engine& e) {

    PROLOG(e)

    srand((unsigned)time(nullptr) + 228);

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

    // class
    lines       = std::make_unique<Lines>(&line);
    rand_lines  = std::make_unique<Lines>(&line);
    sparks      = std::make_unique<Sparks>();
    lazers      = std::make_unique<Lazers>(e, &line);
    ballTail    = std::make_unique<Tail>(&line, .7f);
    cursorTail  = std::make_unique<Tail>(&line, .08f);
    ball        = std::make_unique<Ball>(e.window->getLogicalSize());

    lines->Reset();

    // labels
    this->labelMenuTitle = std::make_unique<Label>(&this->large_text, "LinHop", glm::vec2 {
            screenW / 2 - 124, screenH / 2.f - 180.f
    });
    this->labelMenuTitle->setColor(glm::vec4 {0.6f, 0.8f, 1.0f, 1.f});

    this->labelMenuContinue = std::make_unique<Label>(&this->medium_text, "Continue", glm::vec2 {
            screenW / 2 - 120, screenH / 2.f - 40.f
    });

    this->labelMenuStart = std::make_unique<Label>(&this->medium_text, "Start", glm::vec2 {
            screenW / 2 - 72, screenH / 2.f + 20.f
    });

    this->labelMenuSettings = std::make_unique<Label>(&this->medium_text, "Settings", glm::vec2 {
            screenW / 2 - 118, screenH / 2.f + 80.f
    });

    this->labelMenuExit = std::make_unique<Label>(&this->medium_text, "Exit", glm::vec2 {
            screenW / 2 - 56, screenH / 2.f + 140.f
    });

    this->labelMenuHint = std::make_unique<Label>(&this->small_text, "Left or right to change mode", glm::vec2 {
            screenW / 2 - 238, screenH - 40.f
    });
    this->labelMenuHint->setColor(glm::vec4 {0.4f, 0.55f, 0.6f, 1.f});

    this->labelMenuMode = std::make_unique<Label>(&this->small_text, "Classic", glm::vec2 {
            screenW / 2 - 74, screenH / 2.f - 110.f
    });

    this->labelEndgameRestart = std::make_unique<Label>(&this->medium_text, "Retry", glm::vec2 {
            screenW / 2 + 55, screenH / 2.f
    });

    this->labelEndgameScore = std::make_unique<Label>(&this->medium_text, "Score: ", glm::vec2 {
            screenW / 2 - 205, screenH / 2.f - 60.f
    });

    this->labelSettingsTitle = std::make_unique<Label>(&this->large_text, "Settings", glm::vec2 {
            screenW / 2 - 170, screenH / 2.f - 280.f
    });

    this->labelSettingsFx = std::make_unique<Label>(&this->medium_text, "FX: ", glm::vec2 {
            screenW / 2 - 165, screenH / 2.f - 60.f
    });

    this->labelSettingsMusicVolume = std::make_unique<Label>(&this->medium_text, "Volume: ", glm::vec2 {
            screenW / 2 - 150, screenH / 2.f
    });

    this->labelSettingsUnlockResizing = std::make_unique<Label>(&this->medium_text, "Resizing: ", glm::vec2 {
            screenW / 2 - 182, screenH / 2.f + 60.f
    });

#if defined(ANDROID)
    this->labelSettingsUnlockResizing->setColor(COLOR_DISABLED);
#endif

    this->labelSettingsResetStatistics = std::make_unique<Label>(&this->medium_text, "Reset", glm::vec2 {
            screenW / 2 - 76, screenH / 2.f + 120.f
    });

    this->labelSettingsBack = std::make_unique<Label>(&this->medium_text, "Back", glm::vec2 {
            screenW / 2 - 58, screenH / 2.f + 280.f
    });

    this->labelGameScore = std::make_unique<Label>(&this->medium_text, "Score: ", glm::vec2 {
            0.0f, MainScene::mediumTextSize
    });

    this->labelGameFps = std::make_unique<Label>(&this->medium_text, " fps", glm::vec2 {
            screenW - 80.0f, MainScene::mediumTextSize
    });

    /* init clicks */
    pointerX = lastClick[0] = screenW / 2.f;
    pointerY = lastClick[1] = screenH;
    this->prevMousePos = {pointerX, pointerY};

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

void MainScene::suspend(Engine& engine) {

    this->ball->deactivate();
    this->lazers->deactivate();
    this->sparks->deactivate();

    line_unload(&line);

    text_unload(&small_text);
    text_unload(&medium_text);
    text_unload(&large_text);

    audio_pause_all(&audio_engine);
}

void MainScene::resume(Engine& engine) {

    line_load(&this->line);
    line.width = 4.f;

    const char *const fontPath = "fonts/OCRAEXT.TTF";
    text_load(&this->small_text, fontPath, MainScene::smallTextSize);
    text_load(&this->medium_text, fontPath, MainScene::mediumTextSize);
    text_load(&this->large_text, fontPath, MainScene::largeTextSize);

    this->ball->activate();
    this->lazers->activate();
    this->sparks->activate();

    audio_play_all(&audio_engine);
}

bool MainScene::update(Engine& engine) {

    // update background color
    static float bgColorDirection = 0.005f * engine.window->getDeltaTime();
    if (backgroundColor[0] > 0.2f || backgroundColor[0] < 0.0f)
        bgColorDirection = -bgColorDirection;

    backgroundColor[0] += -bgColorDirection / 2;
    backgroundColor[1] += bgColorDirection / 3;
    backgroundColor[2] += bgColorDirection / 2;

    engine.graphics.clear(backgroundColor);

    if (engine.input.isKeyHold(InputKey::PointerMove))
        this->onEventPointerMove(engine);
    if (engine.input.isKeyDown(InputKey::Pointer))
        this->onEventPointerDown();
    if (engine.input.isKeyUp(InputKey::Pointer))
        this->onEventPointerUp(engine);
    if (engine.input.isKeyDown(InputKey::Select))
        this->onEventSelect(engine);
    if (engine.input.isKeyDown(InputKey::Back))
        this->onEventBack();
    if (engine.input.isKeyDown(InputKey::Up))
        this->onEventUp();
    if (engine.input.isKeyDown(InputKey::Down))
        this->onEventDown();
    if (engine.input.isKeyDown(InputKey::Left))
        this->onEventLeft();
    if (engine.input.isKeyDown(InputKey::Right))
        this->onEventRight();

    PROLOG(engine)

    // game logic
    switch (this->gameState) {
        case GameState::MENU:
        case GameState::ENDGAME:
        case GameState::INGAME:

            gameScore = std::max(gameScore, -static_cast<long>(
                ball->pos[1] - screenH / 2.f));

            ball->bounceStrength = 1 + static_cast<float>(gameScore) / ballStrengthMod;
            ball->gravity = 9.8f + static_cast<float>(gameScore) / ballGravityMod;

            ball->Move(engine.window->getDeltaTime());

            if (ball->Collision(*lines, ball->prev_pos)) {
                sparks->Push(ball->pos);
                audio_play(&this->audio_engine, &this->audio_bounce);
            }

            if (ball->Collision(*rand_lines, ball->prev_pos)) {
                sparks->Push(ball->pos);
                audio_play(&this->audio_engine, &this->audio_bounce);
            }

            /* If ball reaches half of the screen then update scroll */
            if (ball->pos[1] - screenH / 2.f - 10.f < scroll) {
                scroll += (ball->pos[1] - screenH / 2.f - 10.f - scroll) / 10.f;
            }

            /* If game was over turn global scroll back */
            if (gameState == GameState::ENDGAME) {
                scroll += (-scroll) / 100;
            }

            /* If the ball is out of screen then stop the game */
            if (ball->pos[0] < 0 || ball->pos[0] > screenW ||
                ball->pos[1] - scroll > screenH + ball->radius)
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
                    auto base_x = t_rand(-(screenW/3), screenW);

                    struct line {
                        glm::vec2 first;
                        glm::vec2 second;
                    } new_line {
                        {base_x, base_y},
                        {base_x + (t_rand(0.f, screenW) / 2.f) - (screenW/2.f) / 4.f,
							base_y + t_rand(0.f, screenH) / 6.f}
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
                        glm::vec2 {pointerX, pointerY + scroll},
                        prevMousePos);
                }
            }

            /* lazers */
            if (gameScore > 1000L) {
                if (t_rand(0, 600) == 1) {
                    audio_play(&this->audio_engine, &this->audio_warning);

                    lazers->Trigger(
                        t_rand(0.0f, screenW - this->lazers->areaWidth));
                }

                if (lazers->liveTime == 59)
                    audio_play(&this->audio_engine, &this->audio_warning);
            }

            break;

        default:
            break;
    }

    // play random music
    if (this->audio_main.state != STATE_BUSY
        && this->audio_alt.state != STATE_BUSY)
    {
        if (rand() % 2 == 0)
            audio_play(&audio_engine, &audio_main);
        else
            audio_play(&audio_engine, &audio_alt);
    }

    return this->gameState != GameState::EXITING;
}

void MainScene::render(Engine& engine) {

    PROLOG(engine)

    lazers->draw();

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

        for (int i = 0; i < screenH; i += screenH / 10) {
            sparks->Push(glm::vec2 {lazers->lazers.back().a[0], static_cast<float>(i)});
            sparks->Push(glm::vec2 {lazers->lazers.front().a[0], static_cast<float>(i)});
        }
    }

    if (saveData.fxEnabled) {
        cursorTail->Draw();
        sparks->Draw();
        ballTail->Draw();
    }

    if (gameMode == GameMode::CLASSIC)
        lines->Draw();

    ball->Draw();

    rand_lines->Draw();

    // gui text
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
                        ->setText("Highest: " + std::to_string(saveData.maxScoreClassic))
                        .setColor(COLOR_IDLE)
                        .draw();

                    this->labelMenuMode
                        ->setText("Classic")
                        .setPos(glm::vec2{ screenW/2 - 60, screenH / 2.f - 110.f })
                        .setColor(COLOR_IDLE)
                        .draw();

                    break;

                case GameMode::HIDDEN:

                    this->labelGameScore
                        ->setText("Highest: " + std::to_string(saveData.maxScoreClassic))
                        .setColor(COLOR_IDLE)
                        .draw();

                    this->labelMenuMode
                        ->setText("Hidden")
                        .setPos(glm::vec2{ screenW/2.f - 52.f, screenH / 2.f - 110.f })
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

            break;

        case GameState::INGAME:

            if (pressed && gameMode == GameMode::CLASSIC) {

                this->line.color[0] = .5f;
                this->line.color[1] = .5f;
                this->line.color[2] = .5f;
                this->line.color[3] = 1.f;

                line_draw(&this->line, &glm::vec4 {lastClick[0], lastClick[1] - scroll, pointerX, pointerY}[0]);
            }

#ifndef NDEBUG

            this->labelGameFps
                ->setText(std::to_string(static_cast<int>(1 / engine.window->getDeltaTime())) + std::string(" fps"))
                .setColor(gameMode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN)
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

    this->prevMousePos = {
        pointerX,
        pointerY + scroll,
    };

    this->pressedOnce = false;
}

void MainScene::reset(Engine& engine) {

    cursorTail->Reset();

    this->prevMousePos = {
        engine.input.getPointerArray()[0][0] / 2.f,
        engine.input.getPointerArray()[0][1]
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

    ball->Reset(engine.window->getLogicalSize());
    rand_lines->Reset();
    lines->Reset();
}

void MainScene::onEventPointerMove(Engine& engine) {

    auto& pointerPos = engine.input.getPointerArray()[0];

    // gestures
    switch (this->gameState) {

        case GameState::MENU:
        case GameState::PAUSED:
            if (this->labelMenuContinue->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->menuSelected = MenuSelected::CONTINUE;
                if (this->pressedOnce) {
                    this->onEventSelect(engine);
                }
            }

            if (this->labelMenuStart->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->menuSelected = MenuSelected::START;
                if (this->pressedOnce) {
                    this->onEventSelect(engine);
                }
            }
            if (this->labelMenuSettings->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->menuSelected = MenuSelected::SETTINGS;
                if (this->pressedOnce) {
                    this->onEventSelect(engine);
                }
            }

            if (this->labelMenuExit->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->menuSelected = MenuSelected::EXIT;
                if (this->pressedOnce) {
                    this->onEventSelect(engine);
                }
            }

            if (this->labelMenuHint->isCollide(glm::make_vec2(pointerPos.data()))) {
                if (this->pressedOnce) {
                    this->labelMenuHint->setColor(glm::vec4 {
                            t_rand(.0f, 1.f), t_rand(.0f, 1.f), t_rand(.0f, 1.f), 1.f
                    });
                }
            }

            if (this->labelMenuMode->isCollide(glm::make_vec2(pointerPos.data()))) {
                if (this->pressedOnce) {
                    this->onEventRight();
                }
            }

            break;

        case GameState::ENDGAME:

            if (this->labelEndgameRestart->isCollide(glm::make_vec2(pointerPos.data()))) {
                if (this->pressedOnce)
                    this->onEventSelect(engine);
            }
            break;

        case GameState::SETTINGS:

            if (this->labelSettingsFx->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->settingsSelected = SettingsSelected::FX_ENABLED;
                if (this->pressedOnce) {
                    this->onEventSelect(engine);
                }
            }

            if (this->labelSettingsMusicVolume->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->settingsSelected = SettingsSelected::MUSIC_VOLUME;
                if (this->pressedOnce) {
                    this->onEventLeft();
                }
            }

            if (this->labelSettingsUnlockResizing->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->settingsSelected = SettingsSelected::UNLOCK_RESIZE;
                if (this->pressedOnce) {
                    this->onEventSelect(engine);
                }
            }

            if (this->labelSettingsResetStatistics->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->settingsSelected = SettingsSelected::RESET_STATISTICS;
                if (this->pressedOnce) {
                    this->onEventSelect(engine);
                }
            }

            if (this->labelSettingsBack->isCollide(glm::make_vec2(pointerPos.data()))) {

                this->settingsSelected = SettingsSelected::BACK;
                if (this->pressedOnce) {
                    this->onEventSelect(engine);
                }
            }
            break;
        default: break;
    }
}

void MainScene::onEventPointerDown() {
    pressed = true;
}

void MainScene::onEventPointerUp(Engine& engine) {

    if (this->gameState == GameState::INGAME) {
        glm::vec2 newPos = {
            engine.input.getPointerArray()[0][0], engine.input.getPointerArray()[0][1] + scroll
        };
        
        this->lines->Push(newPos, lastClick);
        this->lastClick = newPos;
    }

    this->pressed = false;
    this->pressedOnce = true;

    onEventPointerMove(engine);
}

void MainScene::onEventSelect(Engine& engine) {

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
                    reset(engine);
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
            reset(engine);
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

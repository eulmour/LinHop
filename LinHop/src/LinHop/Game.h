#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

enum LinPopState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class LinPop
{
public:
    LinPopState             State;
    unsigned int            Width, Height;
    unsigned int            Level;
    unsigned int            Lives;

    LinPop(unsigned int width, unsigned int height);
    ~LinPop();
    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    void ResetLevel();
    void ResetPlayer();
    void Quit();
};
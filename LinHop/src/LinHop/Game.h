#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "GameObject.h"

enum LinHopState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class LinHop
{
public:
    LinHopState             State;
    unsigned int            Width, Height;
    unsigned int            Level;
    unsigned int            Lives;

    LinHop(unsigned int width, unsigned int height);
    ~LinHop();
    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    void ResetLevel();
    void ResetPlayer();
    void Quit();

private:
    Renderer* renderer;
    CircleObject* player;
};
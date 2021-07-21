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
    unsigned int            Level;
    unsigned int            Lives;

    LinHop(unsigned int width, unsigned int height);
    ~LinHop();
    void Init();
    void Message(int id);
    void Update(float dt);
    void ClearScreen(float dt);
    void Render(float dt);
    void ResetPlayer();
    void Quit();
};

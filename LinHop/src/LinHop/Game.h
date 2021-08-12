#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class LinHop
{
public:
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

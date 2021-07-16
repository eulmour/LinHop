#ifndef RESOURCES_H
#define RESOURCES_H

#include <map>
#include <string>

#include "Shader.h"
#include "Texture.h"

class Resources
{
public:
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture>   Textures;

    static void         LoadShader(const char* filePath, std::string name);
    static Shader&      GetShader(std::string name);
    static void         LoadTexture(const char* file, std::string name, bool alpha = true);
    static Texture&     GetTexture(const std::string name);
    static void         Clear();
private:
    Resources()         { }
};

#endif
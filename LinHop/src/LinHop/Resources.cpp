#include "Resources.h"
#include <gl/glew.h>

// Instantiate static variables
std::map<std::string, Texture>  Resources::Textures;
std::map<std::string, Shader>   Resources::Shaders;

Shader& Resources::LoadShader(const char* filePath, std::string name)
{
    Shaders[name] = Shader(filePath);
    return Shaders[name];
}

Shader& Resources::GetShader(std::string name)
{
    return Shaders[name];
}

void Resources::LoadTexture(const char* file, std::string name, bool alpha)
{
    Textures[name] = Texture(file);
}

Texture& Resources::GetTexture(const std::string name)
{
    return Textures[name];
}

void Resources::Clear()
{
    for (auto shader : Shaders)
        glDeleteProgram(shader.second.GetID());

    for (auto texture : Textures)
    {
        unsigned int id = texture.second.GetID();
        glDeleteTextures(1, &id);
    }
}
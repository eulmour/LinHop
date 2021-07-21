#include "Renderer.h"
#include "GLCall.h"
#include <iostream>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

void Renderer::DrawRect(
	const RectangleObject&  rect,
	glm::vec2		        position,
	glm::vec2		        size,
	float			        rotate,
	glm::vec4		        color)
{
    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<float>(Info.width),
        static_cast<float>(Info.height),
        0.0f,
        -1.0f,
        1.0f);

    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    rect.pShader->Bind();
    rect.pShader->SetUniformMat4f("projection", projection);
    rect.pShader->SetUniformMat4f("model", model);
    rect.pShader->SetUniform4f("spriteColor", color.x, color.y, color.z, color.w);

    rect.Draw();
}

void Renderer::DrawText(
    std::string        text,
    TextObject&         obj,
    glm::vec2			pos,
    glm::vec3			color,
    unsigned int		scale
)
{
    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<float>(Info.width),
        static_cast<float>(Info.height),
        0.0f,
        -1.0f,
        1.0f);

    obj.pShader->Bind();
    obj.pShader->SetUniformMat4f("projection", projection);
    obj.pShader->SetUniform4f("uColor", color.x, color.y, color.z, 1.0f);

    obj.Draw(text, pos, scale);
}

void Renderer::DrawLine(const LineObject& line, glm::vec2 a_pos, glm::vec2 b_pos, glm::vec3 color)
{
    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<float>(Info.width),
        static_cast<float>(Info.height),
        0.0f,
        -1.0f,
        1.0f);

    line.pShader->Bind();
    line.pShader->SetUniformMat4f("projection", projection);
    line.pShader->SetUniform4f("uColor", color.x, color.y, color.z, 1.0f);

    line.Draw(a_pos, b_pos);
}
#include "Game.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include "Resources.h"

//#include "VertexBuffer.h"
//#include "VertexBufferLayout.h"
//#include "IndexBuffer.h"
//#include "VertexArray.h"
//#include "Shader.h"
//#include "Texture.h"
//#include "GUILayer.h"
//
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
//
//#include "../imgui/imgui.h"
//#include "../imgui/imgui_impl_glfw.h"
//#include "../imgui/imgui_impl_opengl3.h"

//Renderer renderer;
//CircleObject player;

LinPop::LinPop(unsigned int width, unsigned int height)
{
}

LinPop::~LinPop()
{
}

void LinPop::Init()
{
	/* Setting shaders */
	Resources::LoadShader("../res/shaders/Basic.shader", "basic");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	Resources::GetShader("basic").Bind().SetUniform1i("u_Texture", 0);
	Resources::GetShader("basic").SetUniformMat4f("projection", projection);

	/* Setting textures */
	Resources::LoadTexture("../res/textures/img.png", "example_img");
}

void LinPop::ProcessInput(float dt)
{
}

void LinPop::Update(float dt)
{
}

void LinPop::Render()
{
}

void LinPop::DoCollisions()
{
}

void LinPop::ResetLevel()
{
}

void LinPop::ResetPlayer()
{
}

void LinPop::Quit()
{
	Resources::Clear();
}

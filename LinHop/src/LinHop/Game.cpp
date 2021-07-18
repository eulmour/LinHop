#include "Game.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include "Resources.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

//#include "../imgui/imgui.h"
//#include "../imgui/imgui_impl_glfw.h"
//#include "../imgui/imgui_impl_opengl3.h"

LinHop::LinHop(unsigned int width, unsigned int height)
	: Width(width),
	Height(height),
	Lives(3),
	Level(1),
	State(LinHopState::GAME_MENU)
{
}

LinHop::~LinHop()
{
	delete renderer;
	delete player;
}

void LinHop::Init()
{
	renderer = new Renderer();
	player = new CircleObject();

	/* Setting shaders */
	Resources::LoadShader("../res/shaders/Basic.shader", "u_Texture");
	Shader& basic = Resources::GetShader("u_Texture");

	//glm::mat4 projection = glm::ortho(
	//	0.0f,
	//	static_cast<float>(this->Width),
	//	static_cast<float>(this->Height),
	//	0.0f,
	//	-1.0f,
	//	1.0f);

	basic.Bind().SetUniform1i("u_Texture", 0);
	//basic.SetUniformMat4f("projection", projection);
	renderer->Use(&basic);
	basic.Unbind();

	/* Setting textures */
	Resources::LoadTexture("../res/textures/img.png", "u_Texture");
}

void LinHop::ProcessInput(float dt)
{
}

void LinHop::Update(float dt)
{
}

void LinHop::Render()
{
	//renderer.Draw(player);

	renderer->DrawTexture(
		Resources::GetTexture("u_Texture"),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(this->Width, this->Height),
		0.0f);
}

void LinHop::DoCollisions()
{
}

void LinHop::ResetLevel()
{
}

void LinHop::ResetPlayer()
{
}

void LinHop::Quit()
{
	Resources::Clear();
}

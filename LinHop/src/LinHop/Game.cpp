#include "Game.h"

#include <list>
#include <string>
#include <cmath>

#include "GLCall.h"
#include "Resources.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#include "GameData.h"
#include "Entities.h"
#include "FileManager.h"

#define LAZERS_LIVE 240
#define LAZERS_WIDTH Info.width / 3

int audioFileInit = (InitAudioPlayer(), 0); /* before any AudioFile is created */
GameData gameData = LoadGameData();

static float last_place = RAND_LINES_DENSITY;

AudioFile musicFiles[] =
{
    AudioFile("../res/audio/a.wav")
};

AudioFile soundFiles[4] =
{
    AudioFile("../res/audio/bounce.wav"),
    AudioFile("../res/audio/warning.wav"),
    AudioFile("../res/audio/fail.wav"),
    AudioFile("../res/audio/fail2.wav"),
};

class Lazers
{
public:
	Lazers() {}

	void Trigger(float position)
	{
		if (liveTime == 0)
		{
			liveTime = LAZERS_LIVE;
			soundFiles[1].playFile();

			lazers.push_back(Lazer({ position, 0.0f }, { position, 720.0f }));
			lazers.push_back(Lazer({ position + (LAZERS_WIDTH), 0.0f }, { position + (LAZERS_WIDTH), 720.0f }));
		}
	}

	void Draw()
	{
		if ((liveTime > 0) && (lazers.size() > 0))
		{
			for (const auto& lazer : lazers)
			{
				 lazer.Draw();
			}

			if (liveTime < 60)
			{
				if (liveTime == 59)
					soundFiles[1].playFile();

				renderer->DrawRect(
					*redRect,
					{ lazers.back().a.x, lazers.back().a.y },
					{ lazers.front().a.x - lazers.back().a.x, Info.height },
					0.0f, { 1.0f, 0.0f, 0.0f, 0.5f }
				);

			}
			
			--liveTime;
		}

		if ((liveTime == 0) && (lazers.size() > 0))
		{
			// lazer will destroy ball

			if (ball.pos.x > lazers.front().a.x && ball.pos.x < lazers.back().a.x)
			{
				if (gameState == GameState::INGAME)
					gameState = GameState::ENDGAME;

				soundFiles[t_rand(2, 3)].playFile();
			}

			sparks.Push({ 0.0f, 0.0f });
			
			for (size_t i = 0; i < Info.height; i += Info.height / 10)
			{
				sparks.Push({ lazers.back().a.x, i });
				sparks.Push({ lazers.front().a.x, i });
			}

			lazers.clear();
		}
	}

	class Lazer
	{
	public:
		glm::vec2 a, b;
		glm::vec4 color;

		Lazer(glm::vec2 a, glm::vec2 b) : a(a), b(b), color({ 1.0f, 0.0f, 0.0f, 1.0f }) {}

		void Draw() const
		{
			renderer->DrawLine(
				*line,
				a, b,
				color
			);
		}
	};

	unsigned int liveTime = 0U;
	float pos;

	std::list<Lazer> lazers;
} lazers;

LinHop::LinHop(unsigned int width, unsigned int height)
{
	Info.width = width;
	Info.height = height;
}

LinHop::~LinHop()
{
	delete renderer;
	delete ballRect;
	delete line;

	DestroyAudioPlayer();
}

void LinHop::Init()
{
	/* Setting shaders */
	Shader& spriteShader	= Resources::LoadShader("../res/shaders/sprite.shader", "sprite");
	Shader& lineShader		= Resources::LoadShader("../res/shaders/line.shader", "line");
	Shader& textShader		= Resources::LoadShader("../res/shaders/text.shader", "text");

	/* Setting textures */
	Resources::LoadTexture("../res/textures/circle.png", "circle");
	Resources::LoadTexture("../res/textures/pixel.png", "pixel");
	Resources::LoadTexture("../res/textures/sparkle.png", "sparkle");

	/* Objects */
	renderer = new Renderer();

	ballRect = new RectangleObject(
		spriteShader,
		{ 0.0f, 0.0f },
		{ Info.width, Info.height },
		Resources::GetTexture("circle"));
	
	sparkRect = new RectangleObject(
		spriteShader,
		{ 0.0f, 0.0f },
		{ Info.width, Info.height },
		Resources::GetTexture("sparkle"));

	redRect = new RectangleObject(
		spriteShader,
		{ 0.0f, 0.0f },
		{ Info.width, Info.height },
		Resources::GetTexture("pixel"));

	line = new LineObject(
		lineShader,
		{ 0.0f, 0.0f },
		{ 100.f, 100.0f },
		{ 1.0f, 1.0f, 1.0f },
		&Resources::GetTexture("pixel"));

	lines.Reset();

	small_text = new TextObject(
		"dummy",
		"../res/fonts/OCRAEXT.TTF",
		textShader,
		glm::vec2(0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		14);

	medium_text = new TextObject(
		"dummy",
		"../res/fonts/OCRAEXT.TTF",
		textShader,
		glm::vec2(0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		22);

	large_text = new TextObject(
		"dummy",
		"../res/fonts/OCRAEXT.TTF",
		textShader,
		glm::vec2(0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		48);

	/* volume settings */
	for (auto& sf : soundFiles)
		sf.volume = 1.0;

	for (auto& mf : musicFiles)
		mf.volume = gameData.musicVolumeFloat;

	/* init clicks */
	lastClick.x = static_cast<float>(Info.width / 2);
	lastClick.y = static_cast<float>(Info.height);
}

void LinHop::Message(int id)
{
	switch (id)
	{
	case GLFW_MOUSE_BUTTON_LEFT:

		if (gameState == GameState::INGAME)
		{
			lines.Push(mousePos, lastClick);
			lastClick = mousePos;
		}

		break;

	case GLFW_KEY_ENTER:

		switch (gameState)
		{
		case GameState::SETTINGS:

			switch (settingsSelected)
			{
			case SettingsSelected::FX_ENABLED:

				gameData.fxEnabled = gameData.fxEnabled ? false : true;

				break; /* end of FX_ENABLED */

			case SettingsSelected::UNLOCK_RESIZE:
			{
				if (gameData.unlockResizing == 1L)
				{
					/* little evil */
					gameData.maxScoreClassic = 0L;
					gameData.maxScoreHidden = 0L;
				}

				gameData.unlockResizing = gameData.unlockResizing == 0L ? 1L : 0L;
				SaveGameData(gameData);

				extern GLFWwindow* window;
				glfwSetWindowShouldClose(window, true);

				break; /* end of UNLOCK_RESIZE */
			}

			case SettingsSelected::RESET_STATISTICS:

				// TODO empty stats
				gameData.maxScoreClassic = 0L;
				gameData.maxScoreHidden = 0L;
				SaveGameData(gameData);

				break; /* end of RESET_STATISTICS */

			case SettingsSelected::BACK:

				gameState = GameState::PAUSED;

				break; /* end of BACK */
			}

			break;

		case GameState::PAUSED:
		case GameState::MENU:

			switch (menuSelected)
			{
			case MenuSelected::START:
				gameState = GameState::INGAME;
				ResetPlayer();
				break;

			case MenuSelected::CONTINUE:
				if (gameState == GameState::PAUSED)
					gameState = GameState::INGAME;
				break;

			case MenuSelected::SETTINGS:
				gameState = GameState::SETTINGS;
				break;

			case MenuSelected::EXIT:
			{
				extern GLFWwindow* window;
				glfwSetWindowShouldClose(window, true);
				break;
			}
			}

			break;
		}

	case GLFW_KEY_R:

		if (gameState == GameState::ENDGAME)
			ResetPlayer();

		break;

	case GLFW_KEY_ESCAPE:

		switch (gameState)
		{
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

		}
		break;

	case GLFW_KEY_UP:

		if (gameState == GameState::MENU)
			--menuSelected;
		if (gameState == GameState::PAUSED)
			--menuSelected;
		if (gameState == GameState::SETTINGS)
			--settingsSelected;

		break;

	case GLFW_KEY_DOWN:

		if (gameState == GameState::MENU)
			++menuSelected;
		if (gameState == GameState::PAUSED)
			++menuSelected;
		if (gameState == GameState::SETTINGS)
			++settingsSelected;

		break;

	case GLFW_KEY_LEFT:

		if (gameState == GameState::MENU)
		{
			--gameMode;
			cursor_tail.alpha = cursor_tail.alpha == 0.15f ? 0.4f : 0.15f;
		}

		if (gameState == GameState::SETTINGS)
			if (settingsSelected == SettingsSelected::MUSIC_VOLUME)
				if (gameData.musicVolumeFloat > 0.0f)
				{
					gameData.musicVolumeFloat -= 0.05f;
					UpdateVolume(musicFiles, gameData.musicVolumeFloat);
				}

		break;
	case GLFW_KEY_RIGHT:

		if (gameState == GameState::MENU)
		{
			++gameMode;
			cursor_tail.alpha = cursor_tail.alpha == 0.08f ? 0.4f : 0.08f;
		}

		if (gameState == GameState::SETTINGS)
			if(settingsSelected == SettingsSelected::MUSIC_VOLUME)
				if (gameData.musicVolumeFloat <= 1.0f)
				{
					gameData.musicVolumeFloat += 0.05f;
					UpdateVolume(musicFiles, gameData.musicVolumeFloat);
				}

		break;

	case 666:
		mousePos.y += scroll;
		break;

	}
}

void LinHop::Update(float dt)
{
	switch (gameState)
	{
	case GameState::MENU:
	case GameState::ENDGAME:
	case GameState::INGAME:

		gameScore = std::max(gameScore, -static_cast<long>((ball.pos.y - Info.height / 2)));
		bounceStrength = 1 + static_cast<float>(gameScore) / BALL_STRENGTH_MOD;
		ball.gravity = 9.8f + static_cast<float>(gameScore) / BALL_GRAVITY_MOD;

		ball.Move(dt);

		/* If ball reaches half of the screen then update scroll */
		if (ball.pos.y - (Info.height / 2 - 10) < scroll)
		{
			scroll += (ball.pos.y - (Info.height / 2 - 10) - scroll) / 10;
		}

		/* If game was over turn global scroll back */
		if (gameState == GameState::ENDGAME)
		{
			scroll += (-scroll) / 100;
		}

		/* If the ball is out of screen then stop the game */
		if (ball.pos.x < 0 || ball.pos.x > Info.width || ball.pos.y - scroll > Info.height + ball.radius)
		{
			if (gameState == GameState::INGAME)
			{
				soundFiles[t_rand(2, 3)].playFile();
				gameState = GameState::ENDGAME;
			}
		}

		/* Random platforms */

		if ((-scroll) - last_place > RAND_LINES_DENSITY)
		{
			if (t_rand(0, 1) <= 1)
			{
				float base_y = scroll - 80.0f;
				float base_x = static_cast<float>(t_rand(0, Info.width));

				struct line { glm::vec2 first; glm::vec2 second; }  new_line;
				new_line.first = { base_x, base_y };
				new_line.second = { base_x + (t_rand(0, Info.width) / 2) - CX / 4, base_y + (t_rand(0, Info.height) / 5) };

				if (dis_func(new_line.second.x - new_line.first.x, new_line.second.y - new_line.first.y) > 30.0f)
				{
					rand_lines.Push(new_line.second, new_line.first, false);
				}
			}

			last_place += RAND_LINES_DENSITY;
		}

		/* Push for tail */

		if (gameData.fxEnabled)
		{
			ball_tail.Push(ball.pos, ball.prev_pos);
			cursor_tail.Push(mousePos, prevMousePos);
		}

		/* lazers */

		if (gameScore > 1000L)
		{
			if (t_rand(0, 600) == 1)
			{
				lazers.Trigger(t_rand(0.0f, static_cast<float>(Info.width - LAZERS_WIDTH)));
			}
		}

		break;
	}

	/* dont put any code below */
	for (AudioFile& file : musicFiles)
	{
		if (file.isPlaying == true)
			return;
	}

	/* play random music */
	musicFiles[t_rand(0, COUNT(musicFiles) - 1)].playFile();
}

void LinHop::ClearScreen(float dt)
{
	static float color[3] = { 0.0f, 0.1f, 0.2f };
	static float direction = 0.0005f * dt;

	if (color[0] > 0.2f || color[0] < 0.0f)
		direction = -direction;

	color[0] += -direction / 2;
	color[1] += direction / 3;
	color[2] += direction / 2;

	glClearColor(color[0], color[1], color[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LinHop::Render(float dt)
{
	switch (gameState)
	{
	case GameState::PAUSED:
	case GameState::MENU:

		renderer->DrawText(
			"LinHop", *large_text,
			{ CX - 85, Info.height / 2 - 180 },
			{ 0.6f, 0.8f, 1.0f }, 1.0f);

		renderer->DrawText(
			"Continue", *medium_text,
			{ CX - 54, Info.height / 2 - 60 },
			menuSelected == MenuSelected::CONTINUE ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		renderer->DrawText(
			"Start", *medium_text,
			{ CX - 34, Info.height / 2 - 20 },
			menuSelected == MenuSelected::START ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		renderer->DrawText(
			"Settings", *medium_text,
			{ CX - 53, Info.height / 2 + 20 },
			menuSelected == MenuSelected::SETTINGS ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		renderer->DrawText(
			"Exit", *medium_text,
			{ CX - 26, Info.height / 2 + 60 },
			menuSelected == MenuSelected::EXIT ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		renderer->DrawText(
			"To change game mode press left or right arrow", *small_text,
			{ CX - 180, Info.height - 20 },
			{ 0.4f, 0.55f, 0.6f }, 1.0f);

		switch (gameMode)
		{
		case GameMode::CLASSIC:

			renderer->DrawText(
			"High score: " + std::to_string(gameData.maxScoreClassic),
			*medium_text,
			{ 0.0f, 5.0f },
			COLOR_IDLE, 1.0f);

			renderer->DrawText(
				"Classic", *small_text,
				{ CX - 29, Info.height / 2 - 130 },
				COLOR_IDLE,
				1.0f);

			break;

		case GameMode::HIDDEN:

			renderer->DrawText(
			"High score: " + std::to_string(gameData.maxScoreHidden),
			*medium_text,
			{ 0.0f, 5.0f },
			COLOR_IDLE, 1.0f);

			renderer->DrawText(
				"Hidden", *small_text,
				{ CX - 25, Info.height / 2 - 130 },
				COLOR_IDLE,
				1.0f);

			break;
		}

		break;

	case GameState::SETTINGS:

		renderer->DrawText(
			"Settings", *large_text,
			{ CX - 110, Info.height / 2 - 180 },
			{ 0.6f, 0.9f, 1.0f }, 1.0f);

		renderer->DrawText(
			CCAT("FX: ", gameData.fxEnabled, "enabled", "disabled"), *medium_text,
			{ CX - 75, Info.height / 2 - 60 },
			settingsSelected == SettingsSelected::FX_ENABLED ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		renderer->DrawText(
			"Music volume: " + std::to_string(static_cast<int>(gameData.musicVolumeFloat * 100)), *medium_text,
			{ CX - 100, Info.height / 2 - 20 },
			settingsSelected == SettingsSelected::MUSIC_VOLUME ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		renderer->DrawText(
			CCAT("Unlock resizing: ", gameData.unlockResizing, "yes", "no"), *medium_text,
			{ CX - 120, Info.height / 2 + 20 },
			settingsSelected == SettingsSelected::UNLOCK_RESIZE ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		renderer->DrawText(
			"Reset game statistics", *medium_text,
			{ CX - 140, Info.height / 2 + 60 },
			settingsSelected == SettingsSelected::RESET_STATISTICS ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		renderer->DrawText(
			"Back", *medium_text,
			{ CX - 27, Info.height / 2 + 100 },
			settingsSelected == SettingsSelected::BACK ? COLOR_SELECTED : COLOR_IDLE,
			1.0f);

		break;

	case GameState::INGAME:

		if (gameMode == GameMode::CLASSIC)
		{
			renderer->DrawLine(
				*line,
				{ lastClick.x, lastClick.y - scroll },
				{ mousePos.x, mousePos.y - scroll },
				{ 0.5f, 0.5f, 0.5f, 1.0f });
		}

#ifdef DEBUG
		renderer->DrawText(
			std::to_string(static_cast<int>(1 / dt)) + std::string(" fps"),
			*medium_text,
			{ Info.width - 80.0f, 5.0f },
			gameMode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN,
			1.0f
		);
#endif
		renderer->DrawText(
			"Score: " + std::to_string(gameScore),
			*medium_text,
			{ 0.0f, 5.0f },
			gameMode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN,
			1.0f);

		break;

	case GameState::ENDGAME:

		renderer->DrawText(
			"Score: " + std::to_string(gameScore),
			*large_text,
			{ CX - 158, Info.height / 2 - 60 },
			gameMode == GameMode::CLASSIC ? COLOR_SELECTED : COLOR_HIDDEN,
			1.0f);

		renderer->DrawText(
			"Press R",
			*medium_text,
			{ CX - 44, Info.height / 2 + 200 },
			{ 1.0f, 0.8f, 0.6f }, 1.0f );

		break;
	}

	lazers.Draw();
	ball.Draw();

	if (gameMode == GameMode::CLASSIC)
		lines.Draw();

	if (gameData.fxEnabled)
	{
		cursor_tail.Draw();
		sparks.Draw();
		ball_tail.Draw();
	}

	rand_lines.Draw();

	prevMousePos = mousePos;
}

void LinHop::ResetPlayer()
{
	cursor_tail.Reset();

	mousePos = { Info.width / 2, Info.height };
	prevMousePos = lastClick = mousePos;
	scroll = 0.0f;

	if (gameMode == GameMode::CLASSIC)
		if (gameScore > gameData.maxScoreClassic)
			gameData.maxScoreClassic = gameScore;

	if (gameMode == GameMode::HIDDEN)
		if (gameScore > gameData.maxScoreHidden)
			gameData.maxScoreHidden = gameScore;

	gameScore = 0L;
	bounceStrength = 1;
	bounceCooldown = 0;
	terminalVelocity = 300 + gameScore / TERMINAL_VEL_MOD;
	gameState = GameState::INGAME;
	last_place = RAND_LINES_DENSITY;

	ball.Reset();
	lines.Reset();
	rand_lines.Reset();
}

void LinHop::Quit()
{
	if (gameData.unlockResizing == 0L)
		SaveGameData(gameData);

	Resources::Clear();
}

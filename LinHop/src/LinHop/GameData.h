#include <type_traits>
#include "GameObject.h"
#include "SoundPlayer.h"
#include "Utils.h"
#include "Renderer.h"

/* colors */
#define COLOR_IDLE glm::vec3{ 0.4f, 0.55f, 0.6f }
#define COLOR_SELECTED glm::vec3{ 0.6f, 0.9f, 1.0f }
#define COLOR_DISABLED glm::vec3{ 0.2f, 0.35f, 0.4f }
#define COLOR_HIDDEN glm::vec3{ 0.5f, 0.35f, 0.6f }

/* game state params */
/* enum declarations */

enum class GameState
{
    MENU, SETTINGS, INGAME, PAUSED, ENDGAME, END
} gameState = GameState::MENU;

enum class GameMode
{
    CLASSIC, HIDDEN, END
} gameMode = GameMode::CLASSIC;

enum class MenuSelected
{
    CONTINUE, START, SETTINGS, EXIT, END
} menuSelected = MenuSelected::START;

enum class SettingsSelected
{
    FX_ENABLED, MUSIC_VOLUME, UNLOCK_RESIZE, RESET_STATISTICS, BACK, END
} settingsSelected = SettingsSelected::FX_ENABLED;

/* increment / decrement support (C++11) */
template< typename T >
struct my_enum_is_unit_steppable { enum { value = false }; };

// for each type you want the operator(s) to be enabled, do this:
template<> struct my_enum_is_unit_steppable<GameState> { enum { value = true }; };
template<> struct my_enum_is_unit_steppable<GameMode> { enum { value = true }; };
template<> struct my_enum_is_unit_steppable<MenuSelected> { enum { value = true }; };
template<> struct my_enum_is_unit_steppable<SettingsSelected> { enum { value = true }; };

template< typename T >
typename std::enable_if< my_enum_is_unit_steppable< T >::value, T >::type
    operator++(T& value)
{
    value = T((static_cast<int>(value) + 1) % static_cast<int>(T::END));
    return value;
}

template< typename T >
typename std::enable_if< my_enum_is_unit_steppable< T >::value, T >::type
    operator--(T& value)
{
    value = T(mod((static_cast<int>(value) - 1), static_cast<int>(T::END)));
    return value;
}

#define TERMINAL_VEL_MOD 12000
#define BALL_STRENGTH_MOD 25000
#define BALL_GRAVITY_MOD 8000

/* globals */

Renderer* renderer;
RectangleObject* redRect;
RectangleObject* ballRect;
RectangleObject* sparkRect;
LineObject* line;
TextObject* small_text;
TextObject* medium_text;
TextObject* large_text;

glm::vec2	mousePos(240.0f, 720.0f);
glm::vec2	prevMousePos(240.0f, 720.0f);
glm::vec2	lastClick;
float		scroll = 0.0f;
long		gameScore = 0L;
float		bounceStrength = 1;
int			bounceCooldown = 0;
float		terminalVelocity = 300 + gameScore / TERMINAL_VEL_MOD;

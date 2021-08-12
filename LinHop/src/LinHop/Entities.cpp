#include "Entities.h"
// #include "GameData.h"
#include "Utils.h"
#include "GLCall.h"
#include "GameObject.h"
#include "Renderer.h"
#include "SoundPlayer.h"

/* defined in Game.h */
extern Renderer*        renderer;
extern LineObject*      line;
extern RectangleObject* sparkRect;
extern RectangleObject* ballRect;
extern float            scroll;
extern float            bounceStrength;
extern int	            bounceCooldown;
extern float            terminalVelocity;
extern AudioFile        soundFiles[4];

Sparks::Sparks() {}

void Sparks::Draw()
{
    auto current = aSparks.begin();
    auto end = aSparks.end();

    while (current != end)
    {
        current->Update();
        current->Draw();

        if (current->life == 0)
        {
            aSparks.erase(current++);
            continue;
        }
        ++current;
    }
}

void Sparks::Push(glm::vec2 position)
{
    for (unsigned int i = 0; i < SPARK_AMOUNT; ++i)
    {
        aSparks.push_front(Spark(position));
    }
}

Sparks::Spark::Spark(glm::vec2 pos) :
    pos(pos),
    //color(1.0f),
    color(t_rand(0.0f, 1.0f), t_rand(0.0f, 1.0f), t_rand(0.0f, 1.0f), t_rand(0.0f, 1.0f)),
    size{ t_rand(1.0f, 5.0f), t_rand(1.0f, 5.0f) },
    vel{ t_rand(-20.0f, 20.0f), t_rand(-30.0f, -10.0f) } {}

void Sparks::Spark::Update()
{
    vel.x -= vel.x / 5;
    vel.y = std::min(1.0f, vel.y + SPARK_GRAVITY);
    pos += vel;

    --life;
}

void Sparks::Spark::Draw()
{
    renderer->DrawRect(
        *sparkRect,
        SCROLL(pos),
        size,
        degrees(std::atan2(-vel.x, -vel.y)), /* rotation */
        {color.r, color.g, color.b, (static_cast<float>(life) / SPARK_LIFE)});
}

Lines::Lines() {}

void Lines::Push(glm::vec2 second, glm::vec2 first, bool isCol /* = true */)
{
    lines.push_back(Line(first, second, randColor(1.0f), isCol));
}

void Lines::Draw()
{
    for (const auto& line : lines)
    {
        line.Draw();
    }
}

void Lines::Reset()
{
    lines.clear();

    lines.push_back(Lines::Line( /* First line */
        { 0.0f, static_cast<float>(Info.height) },
        { static_cast<float>(Info.width), static_cast<float>(Info.height) },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        false
    ));
}

Lines::Line::Line(glm::vec2 a_pos, glm::vec2 b_pos, glm::vec4 color, bool isCol /* = true */) :
    bCollinear(isCol),
    color(color),
    circle{ { a_pos, color }, { b_pos, color } }
{
    this->a_pos = a_pos.x < b_pos.x ? a_pos : b_pos;
    this->b_pos = a_pos.x < b_pos.x ? b_pos : a_pos;
}

void Lines::Line::Draw() const
{
    /* No off-screen rendering */
    if (a_pos.y - scroll > Info.height && b_pos.y - scroll > Info.height)
        return;

    renderer->DrawLine(*line, { a_pos.x, a_pos.y - scroll }, { b_pos.x, b_pos.y - scroll }, color);

    if (!bCollinear)
        circle[0].Draw();
    
    circle[1].Draw();
}

Lines::Circle::Circle() : pos(0.0f, 0.0f) {}

Lines::Circle::Circle(glm::vec2 pos, glm::vec4 color) : pos(pos), color(color) {}

void Lines::Circle::Draw() const
{
    float old_x = pos.x;
    float old_y = pos.y - radius;

    // if (rand() % 500 == 0)
    // 	sparks.Push(pos);

    for (size_t i = 0; i <= steps; ++i)
    {
        float new_x = pos.x + radius * sinf(angle * i);
        float new_y = pos.y + -radius * cosf(angle * i);

        renderer->DrawLine(*line, { old_x, old_y - scroll }, { new_x, new_y - scroll }, color);
        old_x = new_x;
        old_y = new_y;
    }
}

Ball::Ball()  : pos({240.0f, 360.0f }) {}

void Ball::Collision(Lines& line_array, glm::vec2& prev_position)
{
    for (const auto& line : reverse(line_array.lines))
    {
        int side = checkLineSides(line.a_pos, line.b_pos, pos);

        if (sign(side) == 1)
        {
            if (intersect(prev_position, pos, line.a_pos, line.b_pos) && bounceCooldown == 0)
            {
                float angle = std::atan2(line.b_pos.y - line.a_pos.y, line.b_pos.x - line.a_pos.x);
                float normal = angle - 3.1415926f * 0.5f;
                float mirrored = mirror_angle(degrees(std::atan2(-vel.y, -vel.x)), degrees(normal));
                float bounce_angle = radians(static_cast<float>(std::fmod(mirrored, 360)));

                vel.x = std::cos(bounce_angle) * (dis_func(vel.x, vel.y) + 100);
                vel.y = std::sin(bounce_angle) * (dis_func(vel.x, vel.y) + 1);
                vel.y -= 300 * bounceStrength;

                sparks.Push(pos);
                bounceCooldown = 3;

                soundFiles[0].playFile();
                break;
            }	
        }
    }
}

void Ball::Move(float dt)
{
    prev_pos = pos;

    /* Update position */
    vel.y = std::min(terminalVelocity, vel.y + gravity);
    pos += vel * dt;

    if (bounceCooldown > 0)
        --bounceCooldown;

    Collision(lines, prev_pos);
    Collision(rand_lines, prev_pos);
}

void Ball::Draw() const
{
    renderer->DrawRect(*ballRect, { pos.x - diameter, pos.y - scroll - radius }, { radius, radius });
}

void Ball::Reset()
{
    pos = { CX, CY };
    vel = { 0.0f, 0.0f };
}

Tail::Tail(const float alpha) : alpha(alpha) {}

void Tail::Push(glm::vec2& a, glm::vec2 b)
{
    aTail.push_front(Line(a, b));
}

void Tail::Draw()
{
    auto current = aTail.begin();
    auto end = aTail.end();

    while (current != end)
    {
        current->Draw(*this);
        current->Update();

        if (current->lifeTime == 0)
        {
            aTail.erase(current++);
            continue;
        }

        ++current;
    }
}

void Tail::Reset()
{
    aTail.clear();
}

Tail::Line::Line(glm::vec2 a, glm::vec2 b) : a_pos(a), b_pos(b) {}

void Tail::Line::Update()
{
    float amount = (((TAIL_LENGTH - static_cast<float>(lifeTime)) * TAIL_FUZZ) / TAIL_LENGTH) / 2;

    a_pos.x += t_rand(-amount, amount);
    a_pos.y += t_rand(-amount, amount);
    b_pos.x += t_rand(-amount, amount);
    b_pos.y += t_rand(-amount, amount);

    --lifeTime;
}

void Tail::Line::Draw(const Tail& tail_ref) const
{
    renderer->DrawLine(
        *line,
        { a_pos.x, a_pos.y - scroll },
        { b_pos.x, b_pos.y - scroll },
        randColor((static_cast<float>(lifeTime) / TAIL_LENGTH) * tail_ref.alpha, 0.15f));
}

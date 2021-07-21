#shader vertex
#version 420 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texture_coord;

out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    vTexCoord = texture_coord;
    gl_Position = projection * model * vec4(position, 0.0, 1.0);
}

#shader fragment
#version 420 core

layout(location = 0) out vec4 color;

in vec2 vTexCoord;

uniform sampler2D sprite;
uniform vec4 spriteColor;

void main()
{
    color = spriteColor * texture(sprite, vTexCoord);
}
#shader vertex
#version 330 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texture_coord;

out vec2 vTexCoord;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex, 0.0, 1.0);
    vTexCoord = texture_coord;
}

#shader fragment
#version 330 core

in vec2 vTexCoord;

uniform sampler2D text;
uniform vec4 uColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, vTexCoord).r);
    gl_FragColor = uColor * sampled;
}
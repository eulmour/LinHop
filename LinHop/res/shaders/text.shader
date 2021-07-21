#shader vertex
#version 420 core

layout(location = 0) in vec4 vertex;

out vec2 vTexCoord;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    vTexCoord = vertex.zw;
}

#shader fragment
#version 420 core

in vec2 vTexCoord;

uniform sampler2D text;
uniform vec4 uColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, vTexCoord).r);
    gl_FragColor = uColor * sampled;
}
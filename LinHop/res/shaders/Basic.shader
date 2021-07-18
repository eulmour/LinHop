#shader vertex
#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;
uniform mat4 u_MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	//gl_Position = projection * view * model * position;
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
};

#shader fragment
#version 420 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

//uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord);
	color = texColor;
};
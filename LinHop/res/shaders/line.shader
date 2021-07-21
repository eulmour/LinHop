#shader vertex
#version 330 core

layout (location = 0) in vec2 position;

out vec2 vertexPos;
uniform mat4 projection;

void main()
{
   gl_Position = projection * vec4(position, 0.0, 1.0);
   vertexPos = position;
}

#shader fragment
#version 330 core

in vec2 vertexPos;
uniform vec4 uColor;
uniform sampler2D uTexture;

void main()
{
    //float Pixels = 64.0;
    //float dx = 15.0 * (1.0 / Pixels);
    //float dy = 10.0 * (1.0 / Pixels);
    //vec2 newUV = vec2(dx * floor(vertexPos.x / dx), dy * floor(vertexPos.y / dy));

    vec2 newUV = floor(vertexPos * 160.0) / 160.0;

	gl_FragColor = uColor * texture2D(uTexture, vertexPos);
}

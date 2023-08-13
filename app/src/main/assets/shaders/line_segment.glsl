#shader vertex

layout(location = 0) in vec2 position;

void main() {
	mat4 projection = ortho(0.0, u_res.x, u_res.y, 0.0, -1.0, 1.0);
	gl_Position = projection * vec4(position, 0.0, 1.0);
}

#shader fragment

void main() {
	out_color = u_color;
}

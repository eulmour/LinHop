#shader vertex

layout(location = 0) in vec2 position;

void main() {
	mat4 projection = ortho(0.0, u_res.x, u_res.y, 0.0, -1.0, 1.0);
	gl_Position = projection * vec4(position, 0.0, 1.0);
}

#shader fragment

uniform vec4 u_position;

void main() {
	vec2 fragCoord = vec2(gl_FragCoord.x, u_res.y - gl_FragCoord.y);
	float lineLength = distance(u_position.xy, u_position.zw);
	vec2 vCenter = vec2((u_position.x + u_position.z) / 2.0, (u_position.y + u_position.w) / 2.0);

	if (distance(vCenter, fragCoord) > (lineLength/2.0) - 26.0) {
		out_color = vec4(0.0, 0.0, 0.0, 0.0);
	} else {
		out_color = u_color;
	}
}

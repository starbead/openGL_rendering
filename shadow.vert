#version 410 core

layout(location=0) in vec3 in_Position;

uniform mat4 shadowMVP;

out vec4 Pos;

void main(void) {
	gl_Position = shadowMVP * vec4(in_Position, 1);
	Pos = gl_Position;
}

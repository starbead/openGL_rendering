#version 410 core

in vec4 Pos;

out vec4 out_Color;

void main(void){
	out_Color = vec4(vec3(Pos.z/Pos.w), 1);
}

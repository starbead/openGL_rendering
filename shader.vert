#version 410 core

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal; 
layout(location=2) in vec2 in_TexCoord;

uniform mat4 viewMat = mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,-3,1);
uniform mat4 projMat = mat4(1.299038, 0, 0, 0, 0, 1.732051, 0, 0, 0, 0, -1.002002, -1.0, 0, 0, -0.2002, 0);
uniform mat4 rotation;
uniform mat4 shadowMVP;

out vec3 normal;
out vec4 worldPos;
out vec2 texCoord;
out vec4 shadowPos;

void main(void) {
	texCoord = vec2(in_TexCoord.x, 1 - in_TexCoord.y);
	worldPos = rotation * vec4(in_Position, 1.0);
	normal = normalize(rotation * vec4(in_Normal, 0)).xyz;
	gl_Position = projMat * viewMat * worldPos;

	shadowPos = shadowMVP * vec4(in_Position, 1.0);
}

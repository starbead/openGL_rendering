#include "pch.h"
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#pragma comment(lib, "glew32")
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "toys.h"
#include "j3a.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;
using namespace std;

int buttonPressed = false;
double lastX, lastY;
vec3 CameraPos = vec3(0, 0, 5);
vec3 lightPos = vec3(5, 5, 5);
float theta = 0, phi = 0, fov = 60;

void render(GLFWwindow* window);
void init();

void CursorPosFunc(GLFWwindow* window, double x, double y);
void MouseButFunc(GLFWwindow* window, int button, int state, int mods);
void ScrollFunc(GLFWwindow* window, double, double y);

int main(void)
{
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	GLFWwindow* window = glfwCreateWindow(640, 480, "Starbead", NULL, NULL);
	glfwSetCursorPosCallback(window, CursorPosFunc);
	glfwSetMouseButtonCallback(window, MouseButFunc);
	glfwSetScrollCallback(window, ScrollFunc);
	glfwMakeContextCurrent(window);
	glewInit();
	init();
	while (!glfwWindowShouldClose(window)) {
		render(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLuint vertexBuffer = 0;
GLuint normalBuffer = 0;
GLuint TexCoordBuffer = 0;
GLuint vertexArray = 0;
GLuint elementArray = 0;
GLuint textureID1 = 0;
GLuint textureID2 = 0;
GLuint bumpID = 0;
GLuint shadowTex = 0, shadowDepth = 0, shadowFBO = 0;

Program prog;
Program ShadowProg;

void init() {
	prog.loadShaders("shader.vert", "shader.frag");
	ShadowProg.loadShaders("shadow.vert", "shadow.frag");
	loadJ3A("Archive/medievalHouse.j3a");

	glGenVertexArrays(1, &vertexArray);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, nVertices[0] * sizeof(vec3), vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, nVertices[0] * sizeof(vec3), normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &TexCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, TexCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, nVertices[0] * sizeof(vec2), texCoords[0], GL_STATIC_DRAW);


	glBindVertexArray(vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, TexCoordBuffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, nullptr);

	glGenBuffers(1, &elementArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArray);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTriangles[0] * sizeof(u32vec3), triangles[0], GL_STATIC_DRAW);

	int w = 0, h = 0, n = 0;

	//Diffuse Texture
	void* buf = stbi_load("Archive/medievalHouseDiff.jpg", &w, &h, &n, 4);

	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	stbi_image_free(buf);

	/*
	//bump
	buf = stbi_load("Archive/dwarfB.jpg", &w, &h, &n, 4);

	glGenTextures(1, &bumpID);
	glBindTexture(GL_TEXTURE_2D, bumpID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	stbi_image_free(buf);
	*/

	//Shadow Map
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 2048, 2048, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	
	//shadow Depth
	glGenTextures(1, &shadowDepth);
	glBindTexture(GL_TEXTURE_2D, shadowDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	
	//shadow FBO
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepth, 0);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("FBO Error\n");
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

}

void render(GLFWwindow* window) {
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glViewport(0, 0, 2048, 2048);
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ShadowProg.programID);

	mat4 rotationMatrix = mat4(1);
	rotationMatrix = rotate(-theta, vec3(0, 1, 0)) * rotate(-phi, vec3(1, 0, 0));
	mat4 shadowProjMat = ortho(-2.f, 2.f, -2.f, 2.f, 0.01f, 10.f);
	mat4 shadowViewMat = lookAt(lightPos, vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 shadowMVP = shadowProjMat * shadowViewMat * rotationMatrix;

	GLuint loc = glGetUniformLocation(ShadowProg.programID, "shadowMVP");
	glUniformMatrix4fv(loc, 1, false, value_ptr(shadowMVP));

	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArray);

	glDrawElements(GL_TRIANGLES, nTriangles[0] * 3, GL_UNSIGNED_INT, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

	glViewport(0, 0, width, height);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(prog.programID);

	loc = glGetUniformLocation(prog.programID, "rotation");
	glUniformMatrix4fv(loc, 1, false, value_ptr(rotationMatrix));

	mat4 viewMat = lookAt(CameraPos, vec3(0), vec3(0, 1, 0));

	loc = glGetUniformLocation(prog.programID, "viewMat");
	glUniformMatrix4fv(loc, 1, false, value_ptr(viewMat));

	mat4 projMat = perspective(fov*3.141592f/180, (float)width/(float)height, 0.1f, 100.f);

	loc = glGetUniformLocation(prog.programID, "projMat");
	glUniformMatrix4fv(loc, 1, false, value_ptr(projMat));

	loc = glGetUniformLocation(prog.programID, "cameraPos");
	glUniform3fv(loc, 1, value_ptr(CameraPos));

	loc = glGetUniformLocation(prog.programID, "shadowMVP");
	glUniformMatrix4fv(loc, 1, false, value_ptr(shadowMVP));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	loc = glGetUniformLocation(prog.programID, "diffTex");
	glUniform1i(loc, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumpID);
	loc = glGetUniformLocation(prog.programID, "bumpTex");
	glUniform1i(loc, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	loc = glGetUniformLocation(prog.programID, "shadowMap");
	glUniform1i(loc, 2);

	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArray);

	glDrawElements(GL_TRIANGLES, nTriangles[0] * 3, GL_UNSIGNED_INT, 0);
	
	glfwSwapBuffers(window);
}

void CursorPosFunc(GLFWwindow* window, double x, double y) {
	if (buttonPressed) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		float dx = (x - lastX) / width;
		float dy = (y - lastY) / height;
		
		theta += -dx * 3.141592f;
		phi += -dy * 3.141592f;

		if (phi > 1.57f) {
			phi = 1.57f;
		}
		if (phi < -1.57f) {
			phi = -1.57f;
		}

		CameraPos = vec3(rotate(theta, vec3(0, 1, 0))*rotate(phi, vec3(1, 0, 0))*vec4(0, 0, 5, 1));

		lastX = x;
		lastY = y;
	}
}

void MouseButFunc(GLFWwindow* window, int button, int state, int mods) {
	if (button == GLFW_MOUSE_BUTTON_2) {
		if (state == GLFW_PRESS) {
			buttonPressed = true;
			glfwGetCursorPos(window, &lastX, &lastY);
		}
		else {
			buttonPressed = false;
		}
	}
}

void ScrollFunc(GLFWwindow* window, double, double y) {
	fov += -y * 5;
	if (fov > 170) {
		fov = 170;
	}
	if (fov < 10) {
		fov = 10;
	}
}

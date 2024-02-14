#ifndef WHERE_IS_MY_HEAD_H
#define WHERE_IS_MY_HEAD_H

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Camera/camera.h>
#include <Shader/shader_s.h>

#include <glm/glm.hpp>
#include "state_models.h"

class WhereIsMyHead
{
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;
	const float FOV = 45.0f;
public:
	glm::mat4* model;
	glm::mat4* view;
	glm::mat4* projection;
	Shader* shader;
	unsigned int* VAO;
	Camera camera;
	GameState gameState;

	WhereIsMyHead(glm::mat4* model, glm::mat4* view, glm::mat4* projection, Shader* shader, unsigned int* VAO, Camera camera, GameState gameState) {
		this->model = model;
		this->view = view;
		this->projection = projection;
		this->shader = shader;
		this->VAO = VAO;
		this->camera = camera;
		this->gameState = gameState;
	}

	void start() {
		*view = camera.GetViewMatrix();
		*projection = glm::perspective(glm::radians(FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader->setVec3("color", glm::vec3(1.0, .263, 0.0));

		shader->setMat4("model", *model);
		shader->setMat4("projection", *projection);
		shader->setMat4("view", *view);

		glBindVertexArray(*VAO);

		glDrawArrays(GL_TRIANGLES, 0, 6 * gameState.walls.size());
	}
};

#endif

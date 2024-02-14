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
public:
	Shader* shader;
	unsigned int* VAO;
	Camera* camera;
	GameState* gameState;
	MainState* mainState;

	WhereIsMyHead(Shader* shader, unsigned int* VAO, Camera* camera, GameState* gameState, MainState* mainState) {
		this->shader = shader;
		this->VAO = VAO;
		this->camera = camera;
		this->gameState = gameState;
		this->mainState = mainState;
	}

	void start() {
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(mainState->FOV), (float)mainState->SCR_WIDTH / (float)mainState->SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);
		shader->setVec3("color", glm::vec3(1.0, .263, 0.0));

		shader->setMat4("model", model);
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		glBindVertexArray(*VAO);

		glDrawArrays(GL_TRIANGLES, 0, 6 * gameState->walls.size());
	}
};

#endif

#ifndef MAP_EDITOR_H
#define MAP_EDITOR_H

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Shader/shader_s.h>

#include <glm/glm.hpp>
#include "state_models.h"

class MapEditor {
public:
	Shader* shader;
	MapEditorState* mapEditorState;
	MainState* mainState;

	MapEditor(Shader* shader, MapEditorState* mapEditorState, MainState* mainState) {
		this->shader = shader;
		this->mapEditorState = mapEditorState;
		this->mainState = mainState;
	}

	void start() {
		glm::mat4 projection = glm::ortho(0.0, (double)mainState->SCR_WIDTH, 0.0, (double)mainState->SCR_HEIGHT, -1.0, 1.0);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);

		shader->setMat4("model", model);
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		if (mapEditorState->dots.x != INT_MAX && mapEditorState->dots.y != INT_MAX) {
			shader->setVec3("color", glm::vec3(0.45f, 0.7f, 0.4f));
			if (mainState->drawingLine) {
				glPointSize(10.0);
				glBegin(GL_POINTS);
					glVertex2f(round(mapEditorState->dots.x, mainState->GRID_SIZE), round(mainState->SCR_HEIGHT - mapEditorState->dots.y, mainState->GRID_SIZE));
				glEnd();
				glLineWidth(1.0);
				glBegin(GL_LINE_LOOP);
					glVertex2f(round(mapEditorState->dots.x, mainState->GRID_SIZE), round(mainState->SCR_HEIGHT - mapEditorState->dots.y, mainState->GRID_SIZE));
					glVertex2f(mainState->xMousePos, mainState->SCR_HEIGHT - mainState->yMousePos);
				glEnd();
			}

			for (int i = 0; i < mapEditorState->lines.size(); i++) {
				glLineWidth(5.0);
				glBegin(GL_LINE_LOOP);
					glVertex2f(round(mapEditorState->lines.at(i).x, mainState->GRID_SIZE), round(mainState->SCR_HEIGHT - mapEditorState->lines.at(i).y, mainState->GRID_SIZE));
					glVertex2f(round(mapEditorState->lines.at(i).z, mainState->GRID_SIZE), round(mainState->SCR_HEIGHT - mapEditorState->lines.at(i).w, mainState->GRID_SIZE));
				glEnd();
			}
		}

		shader->setVec3("color", glm::vec3(1.0, 1.0, 1.0));
		for (int x = 0; x < mainState->SCR_WIDTH; x++) {
			for (int y = 0; y < mainState->SCR_HEIGHT; y++) {
				if (x % mainState->GRID_SIZE == 0 || y % mainState->GRID_SIZE == 0) {
					glLineWidth(1.0);
					glBegin(GL_LINE_LOOP);
						glVertex2f(x, y);
						glVertex2f(x + 1, y + 1);
					glEnd();
				}
			}
		}
	}

private:
	int round(int n, int multiple)
	{
		int a = (n / multiple) * multiple;
		int b = a + multiple;

		return (n - a > b - n) ? b : a;
	}
};

#endif

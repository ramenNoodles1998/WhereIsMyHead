#pragma once
#include <vector>
#include <glm/glm.hpp>

struct MainState {
	bool firstMouse = true;
	bool enter3D = false;
	bool drawingLine = false;
	bool switchMode = false;

	float lastX = 800.0 / 2, lastY = 600.0 / 2;
	double xMousePos, yMousePos;
	double xMousePosClick, yMousePosClick;

	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;
	const float FOV = 45.0f;
	const int GRID_SIZE = 30;
} mainState;

struct MapEditorState {
	glm::vec2 dots = glm::vec2(INT_MAX, INT_MAX);
	std::vector<glm::vec4> lines;
} mapEditorState;

struct GameState {
	std::vector<glm::vec4> walls;
} gameState;


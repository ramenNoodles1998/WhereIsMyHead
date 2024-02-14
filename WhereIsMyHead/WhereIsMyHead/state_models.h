#pragma once
#include <vector>
#include <glm/glm.hpp>

struct MainState {
	bool firstMouse = true;
	bool enter3D = false;
	bool drawingLine = false;
	bool switchMode = false;
} mainState;

struct MapEditorState {
	glm::vec2 dots = glm::vec2(INT_MAX, INT_MAX);
	std::vector<glm::vec4> lines;
} mapEditorState;

struct GameState {
	std::vector<glm::vec4> walls;
} gameState;


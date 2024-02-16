#pragma once
#include <vector>
#include <glm/glm.hpp>

class Position {
public:
	int x;
	int y;

	Position() {
		x = INT_MAX;
		y = INT_MAX;
	}

	Position(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

struct Line {
	Position position;
	Position secondPosition;
};

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
	const int GRID_SIZE = 100;
} mainState;

class MapEditorState {
public:
	Position dots;
	std::vector<Line> lines;

	MapEditorState() {
		Position p;
		dots = p;
	}

	MapEditorState(Position dots) {
		this->dots = dots;
	}
};

struct GameState {
	std::vector<glm::vec4> walls;
} gameState;


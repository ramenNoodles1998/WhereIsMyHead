#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include <Camera/camera.h>
#include <Shader/shader_s.h>

#include <glm/glm.hpp>
#include "state_models.h"
#include "where_is_my_head.h"
#include "map_editor.h"

int main();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, int* keyOneOldState);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int init();
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);

float* environmentVertices = (float*)malloc(18 * sizeof(float));
bool environmentVerticesSet = false;
GLFWwindow* window;
int main() {
	int status = init();
	if (status < 0) {
		printf("initialization failed\n");
		return -1;
	}

	Shader floorShader("vFloorShader.txt", "fFloorShader.txt");

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	int oldState = GLFW_PRESS;
	GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CURSOR);
	
	MapEditor mapEditor(&floorShader, &mapEditorState, &mainState);

	WhereIsMyHead whereIsMyHead(&floorShader, &VAO, &camera, &gameState, &mainState);

	
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, &oldState);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		floorShader.use();

		if (!mainState.switchMode) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			whereIsMyHead.start();
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursor(window, cursor);
			mapEditor.start();
			mainState.enter3D = true;
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyCursor(cursor);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	free(environmentVertices);

	return 0;
}

void processInput(GLFWwindow* window, int* oldState) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE && *oldState == GLFW_PRESS)
		mainState.switchMode = !mainState.switchMode;
	*oldState = glfwGetKey(window, GLFW_KEY_1);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (mainState.drawingLine && mainState.switchMode) {
			glm::vec4 positionVector = glm::vec4(mapEditorState.dots, mainState.xMousePos, mainState.yMousePos);
			mapEditorState.lines.push_back(positionVector);
			mainState.drawingLine = false;
			glm::vec4 nPositionVector = glm::vec4(
				(mainState.SCR_WIDTH - mapEditorState.dots.x) / mainState.GRID_SIZE,
				(mainState.SCR_HEIGHT - mapEditorState.dots.y) / mainState.GRID_SIZE,
				(mainState.SCR_WIDTH - mainState.xMousePos) / mainState.GRID_SIZE,
				(mainState.SCR_HEIGHT - mainState.yMousePos) / mainState.GRID_SIZE
			);

			gameState.walls.push_back(nPositionVector);
			printf("finished line x: %f, z: %f, x: %f, z: %f \n", mapEditorState.dots.x, mapEditorState.dots.y, mainState.xMousePos, mainState.yMousePos);
			//create array for each wall
			//then loop trhough and create mega vertices.
			int oldWallSize = gameState.walls.size() - 1;
			if (environmentVerticesSet) {
				float* newEnvironmentVertices = (float*)malloc(18 * sizeof(float) * oldWallSize);
				memcpy(&newEnvironmentVertices[0], &environmentVertices[0], sizeof(float) * 18 * oldWallSize);
				free(environmentVertices);
				environmentVertices = (float *)malloc(sizeof(float) * 18 * gameState.walls.size());
				memcpy(&environmentVertices[0], &newEnvironmentVertices[0], 18 * sizeof(float) * oldWallSize);
				free(newEnvironmentVertices);
			}
			else {
				environmentVerticesSet = true;
			}
			//float floorVertices[] = {
		//	-0.5f, -0.5f, 0.0f,
		//	 0.5f, -0.5f, 0.0f,
		//	 0.0f,  0.5f, 0.0f
		//};
			//tinker with this til it makes sense more.
			//think needs to take into account direction drawn.
			//maybe try relative mouse positions, and redo saving points.
			//seperate out 3d mode and build mode 
			float wallVertices[] = {
				nPositionVector.z, 0.0f, nPositionVector.w,
				nPositionVector.x, 0.0f, nPositionVector.y,
				 nPositionVector.x,  1.5f, nPositionVector.y,

				 nPositionVector.z, 1.5f, nPositionVector.w,
				 nPositionVector.z, 0.0f, nPositionVector.w,
				 nPositionVector.x,  1.5f, nPositionVector.y,
			};

			memcpy(&environmentVertices[18 * oldWallSize], wallVertices, sizeof(wallVertices));

			/*for (int i = 0; i < 18 * threeDState.walls.size(); i++)
				printf("%f element =%f", i, environmentVertices[i]);*/

			glBufferData(GL_ARRAY_BUFFER, 18.0f * sizeof(float) * gameState.walls.size(), environmentVertices, GL_STATIC_DRAW);

		}
		else if (mainState.switchMode) {
			mainState.drawingLine = true;
		}

		mainState.xMousePosClick = mainState.xMousePos;
		mainState.yMousePosClick = mainState.yMousePos;

		mapEditorState.dots = glm::vec2(mainState.xMousePosClick, mainState.yMousePosClick);
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	glfwGetCursorPos(window, &mainState.xMousePos, &mainState.yMousePos);
	//we need to save old xposIn from switchingModes and yposIn
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (mainState.firstMouse)
	{
		mainState.lastX = xpos;
		mainState.lastY = ypos;
		mainState.firstMouse = false;
	}

	if (mainState.enter3D) {
		mainState.lastX = xpos;
		mainState.lastY = ypos;
		camera.ProcessMouseMovement(0, 0, true);
		mainState.enter3D = false;
	}

	float xoffset = xpos - mainState.lastX;
	float yoffset = mainState.lastY - ypos;
	mainState.lastX = xpos;
	mainState.lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset, true);
}


int init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(mainState.SCR_WIDTH, mainState.SCR_HEIGHT, "Where is my head", NULL, NULL);

	if (window == NULL)
	{
		printf("Failed to creat winder\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { 
		printf("GLAD IS SAD\n");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}

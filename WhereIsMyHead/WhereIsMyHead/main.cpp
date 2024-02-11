#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include <Camera/camera.h>
#include <Shader/shader_s.h>

#include <glm/glm.hpp>

int main();

int round(int n, int multiple);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, int* keyOneOldState);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void go3dMode(glm::mat4* model, glm::mat4* view, glm::mat4* projection, Shader* shader, unsigned int* VAO);
void goMapEditor(glm::mat4* model, glm::mat4* view, glm::mat4* projection, Shader* shader);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

const float FOV = 45.0f;

float lastX = 800.0 / 2, lastY = 600.0 / 2;
double xMousePos, yMousePos;
double xMousePosClick, yMousePosClick;

class state {
public:
	bool firstMouse = true;
	bool enter3D = false;
	bool drawingLine = false;
	bool switchMode = false;
} state;

class triangle {
public:
	glm::vec3 pointOne;
	glm::vec3 pointTwo;
	glm::vec3 pointThree;
} triangleSet;

class buildingModeState {
public:
	glm::vec2 dots = glm::vec2(INT_MAX, INT_MAX);
	std::vector<glm::vec4> lines;
	std::vector<triangle> triangles;
	int clickCount = 0;
} buildingModeState;



int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Where is my head", NULL, NULL);

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

	Shader floorShader("vFloorShader.txt", "fFloorShader.txt");

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	int oldState = GLFW_PRESS;
	GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CURSOR);
	
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, &oldState);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		floorShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		if (!state.switchMode) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			go3dMode(&model, &view, &projection, &floorShader, &VAO);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursor(window, cursor);
			goMapEditor(&model, &view, &projection, &floorShader);
			state.enter3D = true;
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyCursor(cursor);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

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
		state.switchMode = !state.switchMode;
	*oldState = glfwGetKey(window, GLFW_KEY_1);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (state.drawingLine) {
			buildingModeState.lines.push_back(glm::vec4(buildingModeState.dots, xMousePos, yMousePos));
			state.drawingLine = false;
		}
		else {
			buildingModeState.clickCount++;
			state.drawingLine = true;
		}

		xMousePosClick = xMousePos;
		yMousePosClick = yMousePos;
		printf("%i", buildingModeState.clickCount);

		if (buildingModeState.clickCount == 1) triangleSet.pointOne = glm::normalize(glm::vec3((float)xMousePosClick, (float)yMousePosClick, 0.0f));
		if (buildingModeState.clickCount == 2) triangleSet.pointTwo = glm::normalize(glm::vec3((float)xMousePosClick, (float)yMousePosClick, 0.0f));
		if (buildingModeState.clickCount == 3) {
			triangleSet.pointThree = glm::normalize(glm::vec3((float)xMousePosClick, (float)yMousePosClick, 0.0f));
			buildingModeState.triangles.push_back(triangleSet);
			buildingModeState.clickCount = 0;
			//float floorVertices[] = {
			//	-0.5f, -0.5f, 0.0f,
			//	 0.5f, -0.5f, 0.0f,
			//	 0.0f,  0.5f, 0.0f
			//};
			float object[] = {
				-triangleSet.pointOne.x, -triangleSet.pointOne.y, triangleSet.pointOne.z,
				triangleSet.pointTwo.x, -triangleSet.pointTwo.y, triangleSet.pointTwo.z,
				triangleSet.pointThree.x, triangleSet.pointThree.y, triangleSet.pointThree.z,
			};
			glBufferData(GL_ARRAY_BUFFER, sizeof(object), object, GL_STATIC_DRAW);
		}

		buildingModeState.dots = glm::vec2(xMousePosClick, yMousePosClick);
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	glfwGetCursorPos(window, &xMousePos, &yMousePos);
	//we need to save old xposIn from switchingModes and yposIn
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (state.switchMode) {

		if (state.firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			state.firstMouse = false;
		}

		if (state.enter3D) {
			lastX = xpos;
			lastY = ypos;
			camera.ProcessMouseMovement(0, 0, true);
			state.enter3D = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		camera.ProcessMouseMovement(xoffset, yoffset, true);
	}
}

void go3dMode(glm::mat4* model, glm::mat4* view, glm::mat4* projection, Shader* shader, unsigned int* VAO) {
	*model = glm::rotate(*model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
	*view = camera.GetViewMatrix();
	*projection = glm::perspective(glm::radians(FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader->setVec3("color", glm::vec3(1.0, .263, 0.0));

	shader->setMat4("model", *model);
	shader->setMat4("projection", *projection);
	shader->setMat4("view", *view);

	glBindVertexArray(*VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void goMapEditor(glm::mat4* model, glm::mat4* view, glm::mat4* projection, Shader* shader) {
	*projection = glm::ortho(0.0, (double)SCR_WIDTH, 0.0, (double)SCR_HEIGHT, -1.0, 1.0);

	shader->setMat4("model", *model);
	shader->setMat4("projection", *projection);
	shader->setMat4("view", *view);

	if (buildingModeState.dots.x != INT_MAX && buildingModeState.dots.y != INT_MAX) {
		shader->setVec3("color", glm::vec3(1.0, .263, 0.0));
		
		if (state.drawingLine) {
			glPointSize(10.0);
			glBegin(GL_POINTS);
				glVertex2f(round(buildingModeState.dots.x, 30), round(SCR_HEIGHT - buildingModeState.dots.y, 30));
			glEnd();
			glLineWidth(1.0);
			glBegin(GL_LINE_LOOP);
				glVertex2f(round(buildingModeState.dots.x, 30), round(SCR_HEIGHT - buildingModeState.dots.y, 30));
				glVertex2f(xMousePos, SCR_HEIGHT - yMousePos);
			glEnd();
		}

		for (int i = 0; i < buildingModeState.lines.size(); i++) {
			glLineWidth(1.0);
			glBegin(GL_LINE_LOOP);
				glVertex2f(round(buildingModeState.lines.at(i).x, 30), round(SCR_HEIGHT - buildingModeState.lines.at(i).y, 30));
				glVertex2f(round(buildingModeState.lines.at(i).z, 30), round(SCR_HEIGHT - buildingModeState.lines.at(i).w, 30));
			glEnd();
		}
	}

	shader->setVec3("color", glm::vec3(0.0, 0.0, 0.0));
	for (int x = 0; x < SCR_WIDTH; x++) {
		for (int y = 0; y < SCR_HEIGHT; y++) {
			if (x % 30 == 0 || y % 30 == 0) {
				glLineWidth(1.0);
				glBegin(GL_LINE_LOOP);
					glVertex2f(x, y);
					glVertex2f(x + 1, y + 1);
				glEnd();
			}
		}
	}
}

int round(int n, int multiple)
{
	// Smaller multiple 
	int a = (n / multiple) * multiple;

	// Larger multiple 
	int b = a + multiple;

	// Return of closest of two 
	return (n - a > b - n) ? b : a;
}

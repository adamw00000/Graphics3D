#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>
#include <cmath>

#define NUM_CAMERAS 2

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
AbstractCamera* GetCamera();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera fpsCamera(glm::vec3(0.0f, -1.0f, 3.0f));
CarCamera carCamera; 

Model carModel;

float lastX = SCR_WIDTH / 2.0f;
int cameraId = 1;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Graphics 3D", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("model.vertex.shader", "model.fragment.shader");

	// load models
	// -----------
	//Model ourModel("Models/Cars/Low_Poly_City_Cars.obj");
	//Model ourModel("Models/Mercedes/Mercedes-Benz CL600 2007 OBJ.obj");
	//Model ourModel("Models/nanosuit/nanosuit.obj");

	carModel = Model("Models/Mustang/mustang_GT.obj");
	carModel.position = glm::vec3(8.8f, -1.77f, 0.0f);
	carModel.rotation = -12.0f;

	//Model streetModel("Models/Street environment/Street environment_V01.obj");
	//Model streetModel("Models/city/gmae.obj");
	//Model streetModel("Models/metro/Metro_1.3ds");
	Model streetModel("Models/Track01/track01_.3ds");
	//Model streetModel("Models/Camellia City/OBJ/Camellia City.obj");

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//carCamera.SetYawPitch(-90.0f, -20);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		//carCamera.SetYawPitch(-90.0f - carModel.rotation, -20);

		// per-frame time logic
		// --------------------
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//camera

		// don't forget to enable shader before setting uniforms
		ourShader.use();
		
		//spotlight + attenuation
		//ourShader.setVec3("spotLight.position", camera->Position);
		//ourShader.setVec3("spotLight.direction", camera->Front);
		//ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		//ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));
		//ourShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
		//ourShader.setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
		//ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		//ourShader.setFloat("spotLight.constant", 1.0f);
		//ourShader.setFloat("spotLight.linear", 0.09f);
		//ourShader.setFloat("spotLight.quadratic", 0.032f);
		
		//directional light
		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		ourShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
		ourShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

		// render the loaded model
		glm::mat4 carModelMatrix = glm::mat4(1.0f);
		carModelMatrix = glm::translate(carModelMatrix, carModel.position); // translate it down so it's at the center of the scene
		carModelMatrix = glm::rotate(carModelMatrix, glm::radians(carModel.rotation), glm::vec3(0.0f, 1.0f, 0.0f));	// rotation
		//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down
		glm::mat4 fixedCarModelMatrix = glm::scale(carModelMatrix, glm::vec3(0.007f, 0.007f, 0.007f));	// it's a bit too big for our scene, so scale it down
		fixedCarModelMatrix = glm::rotate(fixedCarModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// rotation
		ourShader.setMat4("model", fixedCarModelMatrix);

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(fixedCarModelMatrix)));
		ourShader.setMat3("normalMatrix", fixedCarModelMatrix);

		// camera
		carCamera.SetCarPosition(carModel.position, carModel.rotation);
		carCamera.SetCarPosition(carModel.position, carModelMatrix);
		AbstractCamera* camera = GetCamera();
		ourShader.setVec3("viewPos", camera->Position);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera->GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		carModel.Draw(ourShader);

		// road model
		glm::mat4 streetModelMatrix = glm::mat4(1.0f);
		streetModelMatrix = glm::translate(streetModelMatrix, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		streetModelMatrix = glm::scale(streetModelMatrix, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down
		streetModelMatrix = glm::rotate(streetModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// rotation
		ourShader.setMat4("model", streetModelMatrix);

		ourShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

		streetModel.Draw(ourShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	const float MovementSpeed = 2.5f;

	glm::mat4 transform1 = glm::mat4(1.0f);
	transform1 = glm::rotate(transform1, glm::radians(carModel.rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 transform2 = glm::mat4(1.0f);
	transform2 = glm::translate(transform2, carModel.position);
	float distance = 1.0f;
	glm::vec3 Front = glm::vec3(transform1 * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));

	float velocity = MovementSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		carModel.position += Front * velocity;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		carModel.position -= Front * velocity;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		carModel.rotation += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		carModel.rotation -= 1.0f;

	if (cameraId == 0)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			fpsCamera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			fpsCamera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			fpsCamera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			fpsCamera.ProcessKeyboard(RIGHT, deltaTime);
	}

	static int cKeyState = GLFW_RELEASE;

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && cKeyState == GLFW_RELEASE)
	{
		cKeyState = GLFW_PRESS;
		cameraId = (cameraId + 1) % NUM_CAMERAS;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
	{
		cKeyState = GLFW_RELEASE;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (cameraId != 0)
		return;
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float)xpos;
	lastY = (float)ypos;

	fpsCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (cameraId == 0)
	{
		fpsCamera.ProcessMouseScroll((float)yoffset);
	}
	else if (cameraId == 1)
	{
		carCamera.ProcessMouseScroll((float)yoffset);
	}
}

AbstractCamera* GetCamera()
{
	if (cameraId == 0)
	{
		return &fpsCamera;
	}
	else if (cameraId == 1)
	{
		return &carCamera;
	}
	return nullptr;
}
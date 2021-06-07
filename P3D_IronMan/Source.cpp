#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL\glew.h>

#include <GLFW\glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm\gtc\matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "Spotlight.h"
#include "AmbientLight.h"
#include "Skybox.h"

#define MSAA_SAMPLES 4

// Functions
void initialize();

// Variables
Camera *mainCamera;
double deltaTime;
double prevTime = 0;
int width = 800;
int height = 600;

// Lights
AmbientLight ambientLight(glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f);

DirectionalLight directionalLight(
	glm::vec3(-2.0f, -5.0f, 0.10f), // direction
	glm::vec3(1.0f, 1.0f, 1.0f), // ambient
	glm::vec3(1.0f, 0.8f, 0.0f) * 3.0f, // diffuse
	glm::vec3(1.0f, 1.0f, 1.0f)); // specular

PositionalLight pointLight(
	glm::vec3(0.2f, 1.0f, 0.5f), // position
	0.2f, 0.006f, 0.002f, // kc, kl, kq
	glm::vec3(1.0f, 1.0f, 1.0f), // ambient
	glm::vec3(0.0f, 0.50f, 1.0f) * 5.0f, //diffuse
	glm::vec3(1.0f, 1.0f, 1.0f)); // specular

Spotlight spotlight(
	glm::vec3(0.0f, 2.0f, 2.0f), // position
	glm::vec3(0.0f, 0.0f, -1.0f), // direction
	0.02f, 0.001f, 0.0005f, // kc, kl, kq
	glm::cos(glm::radians(30.0f)), // cutoff
	128, // exponent
	glm::vec3(0.50f, .50f, .50f), // ambient
	glm::vec3(0.3f, 1.0f, 0.3f), // diffuse
	glm::vec3(1.0f, 1.0f, 1.0f)); // specular

// TODO: Remove them
GLuint VAOs[1];
GLuint buffers[3];
GLuint lightShader;
GLuint skyboxShader;

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

int main(void) {

	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	// Sets the sample for the multi sample anti aliasing
	glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLES);

	window = glfwCreateWindow(width, height, "P3D: Iron Man", NULL, NULL);

	if (window == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

	initialize();

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// Callbacks
	glfwSetScrollCallback(window,
		[](GLFWwindow* window, double xOffset, double yOffset) 
		{
			mainCamera->Zoom(yOffset);
		}
	);

	glfwSetWindowSizeCallback(window, 
		[](GLFWwindow* window, int w, int h) 
	{
		glViewport(0, 0, w, h);
		width = w;
		height = h;
	});

	glfwSetKeyCallback(window,
		[](GLFWwindow*, int key, int scancode, int action, int mods)
		{
			if (key == GLFW_KEY_1 && action == GLFW_PRESS)
				ambientLight.FlipSwitch(lightShader);
			if (key == GLFW_KEY_2 && action == GLFW_PRESS)
				directionalLight.FlipSwitch(lightShader);
			if (key == GLFW_KEY_3 && action == GLFW_PRESS)
				pointLight.FlipSwitch(lightShader);
			if (key == GLFW_KEY_4 && action == GLFW_PRESS)
				spotlight.FlipSwitch(lightShader);

			// Deformation
			static bool active = false;

			if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
				GLint deformationId = glGetProgramResourceLocation(lightShader, GL_UNIFORM, "deformation");
				glProgramUniform1f(lightShader, deformationId, !active);
				active = !active;
			}
		});

	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER,   "Shaders/Deformation.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Color.frag" },
		{ GL_NONE, NULL }
	};

	ShaderInfo skyShaders[] = {
		{ GL_VERTEX_SHADER,   "Shaders/Skybox.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Skybox.frag" },
		{ GL_NONE, NULL }
	};

	lightShader = LoadShaders(shaders);
	if (!lightShader)
		exit(EXIT_FAILURE);

	skyboxShader = LoadShaders(skyShaders);
	if (!skyboxShader)
		exit(EXIT_FAILURE);
	

	Light *lights[] = { &ambientLight, &directionalLight, &pointLight, &spotlight };

	for (auto& i : lights)
	{
		i->SetLightProperties(lightShader);
	}

	Model mesh("Iron_Man.obj", lightShader, glm::vec3(0.0f, 0.0f, 0.0f));
	mainCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), mesh.position, glm::vec3(0.0f, 2.0f, 0.0f), float(width) / float(height));

	// Skybox textures
	std::vector<std::string> skyboxTextures{
		"Textures/skyboxRight.jpg",
		"Textures/skyboxLeft.jpg",
		"Textures/skyboxUp.jpg",
		"Textures/skyboxDown.jpg",
		"Textures/skyboxFront.jpg",
		"Textures/skyboxBack.jpg"
	};

	Skybox skybox(skyboxTextures, skyboxShader);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Updating delta time
		double time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;
		
		// Time
		GLint timeId = glGetProgramResourceLocation(lightShader, GL_UNIFORM, "time");
		glProgramUniform1f(lightShader, timeId, (float)time);

		mesh.Render(*mainCamera);
		skybox.Render(*mainCamera);

		// Rotation based on mouse position
		if (glfwGetWindowAttrib(window, GLFW_HOVERED))
		{
			// Areas where the cursor needs to be to rotate
			const int threshold = width * 0.2f;
			const GLfloat MAX_ROTATION = glm::radians(90.0f) * deltaTime;
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			if (mouseX < threshold)
				mainCamera->Rotate((threshold - mouseX) / threshold * MAX_ROTATION);
			else if (mouseX > width - (double)threshold)
				mainCamera->Rotate((width - (double)threshold - mouseX) / threshold * MAX_ROTATION);
		}
		mainCamera->Update();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void initialize() {
	glClearColor(0.68f, 0.85f, 0.90f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	// Enables the multi sample anti aliasing
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_CLAMP);

	Texture::SetFlipVertically(true);
}



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

#define MSAA_SAMPLES 4

// Functions
void initialize();
void display(std::vector<glm::vec3> obj, glm::mat4 mvp);

// Variables
Camera *mainCamera;
double deltaTime;
double prevTime = 0;
float width = 800;
float height = 600;

// Lights
AmbientLight ambientLight(glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f);

DirectionalLight directionalLight(
	glm::vec3(0.0f, 0.0f, -1.0f), // direction
	glm::vec3(1.0f, 1.0f, 1.0f), // ambient
	glm::vec3(1.0f, 1.0f, 1.0f) * 1.0f, // diffuse
	glm::vec3(1.0f, 1.0f, 1.0f)); // specular

PositionalLight pointLight(
	glm::vec3(0.2f, 1.0f, 0.5f), // position
	1.0f, 0.006f, 0.002f, // kc, kl, kq
	glm::vec3(1.0f, 1.0f, 1.0f), // ambient
	glm::vec3(1.0f, 1.0f, 0.1f), //diffuse
	glm::vec3(1.0f, 1.0f, 1.0f)); // specular

Spotlight spotlight(
	glm::vec3(0.0f, 0.50f, 2.0f), // position
	glm::vec3(0.0f, 0.0f, -1.0f), // direction
	0.02f, 0.001f, 0.0005f, // kc, kl, kq
	glm::cos(glm::radians(30.0f)), // cutoff
	60, // exponent
	glm::vec3(0.50f, .50f, .50f), // ambient
	glm::vec3(1.0f, 1.0f, 0.1f), // diffuse
	glm::vec3(1.0f, 1.0f, 1.0f)); // specular

// TODO: Remove them
GLuint VAOs[1];
GLuint buffers[3];
GLuint program;

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

	// TODO: pass the model position as target
	glm::vec3 target(0.0f, 0.0f, 0.0f);
	mainCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), target, float(width) / float(height));
	
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

	Texture::SetFlipVertically(true);

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

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int w, int h) 
		{
			glViewport(0, 0, w, h);
			width = w;
			height = h;
		});

	Model mesh("Iron_Man.obj");

	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER,   "Deformation.vert" },
		{ GL_FRAGMENT_SHADER, "Color.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	if (!program)
		exit(EXIT_FAILURE);
	glUseProgram(program);

	glfwSetKeyCallback(window,
		[](GLFWwindow*, int key, int scancode, int action, int mods)
		{
			if (key == GLFW_KEY_1 && action == GLFW_PRESS)
				ambientLight.FlipSwitch(program);
			if (key == GLFW_KEY_2 && action == GLFW_PRESS)
				directionalLight.FlipSwitch(program);
			if (key == GLFW_KEY_3 && action == GLFW_PRESS)
				pointLight.FlipSwitch(program);
			if (key == GLFW_KEY_4 && action == GLFW_PRESS)
				spotlight.FlipSwitch(program);

			// Deformation
			static bool active = false;

			if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
				GLint deformationId = glGetProgramResourceLocation(program, GL_UNIFORM, "deformation");
				glProgramUniform1f(program, deformationId, !active);
				active = !active;
			}
		});

	// VAO
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[0]);

	// VBOs
	glGenBuffers(3, buffers);

	// Position buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.positions.size(), mesh.positions.data(), 0);
	GLint pos = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vpos");
	GLint bindId = 0;
	glVertexAttribFormat(pos, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(pos, bindId);
	glBindVertexBuffer(bindId, buffers[0], 0, sizeof(GLfloat) * 3);
	glEnableVertexAttribArray(pos);

	// Uv buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvs.size(), mesh.uvs.data(), 0);
	GLint uvId = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vtexcoord");
	bindId = 1;
	glVertexAttribFormat(uvId, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(uvId, bindId);
	glBindVertexBuffer(bindId, buffers[1], 0, sizeof(GLfloat) * 2);
	glEnableVertexAttribArray(uvId);

	// Normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), 0);
	GLint normalId = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vnormal");
	bindId = 2;
	glVertexAttribFormat(normalId, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(normalId, bindId);
	glBindVertexBuffer(bindId, buffers[2], 0, sizeof(GLfloat) * 3);
	glEnableVertexAttribArray(normalId);

	// Textures
	GLint texId = glGetProgramResourceLocation(program, GL_UNIFORM, "image");
	glProgramUniform1i(program, texId, 0);

	// Projection
	GLint projectionId = glGetProgramResourceLocation(program, GL_UNIFORM, "projection");
	glProgramUniformMatrix4fv(program, projectionId, 1, GL_FALSE, glm::value_ptr(mainCamera->GetProjectionMatrix()));

	Light *lights[] = { &ambientLight, &directionalLight, &pointLight, &spotlight };

	for (auto& i : lights)
	{
		i->SetLightProperties(program);
	}

	// Set material in shader
	mesh.material->SetShaderMaterial(program);

    glm::mat4 translation(1.0f);
	glm::mat4 rotation(1.0f);
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Updating delta time
		double time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		//model = glm::rotate(model, (GLfloat)(glm::radians(90.0f) * deltaTime), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 vm = mainCamera->GetViewMatrix() * model;
		glm::mat4 cm = mainCamera->GetViewMatrix() * glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.50f, 2.0f));
		//std::cout << glm::length(glm::vec3(cm[3].x, cm[3].y, cm[3].z)) << std::endl;
		//float dist = glm::length(glm::mat3(vm) * glm::vec3(0.0f, 0.50f, 2.0f) - glm::vec3((vm * glm::vec4(glm::vec3(0,-2,0), 1.0))));
		glm::mat4 viewC = mainCamera->GetViewMatrix();
		glm::vec3 l = glm::vec3(cm[3]);
		glm::vec3 c = glm::vec3(viewC[3]);
		glm::vec3 m = glm::vec3(vm[3]);
		/*float dist = glm::length(l - m);
		std::cout << dist << std::endl;*/
		
		// MVP
		glm::mat4 mvp = mainCamera->GetProjectionMatrix() * mainCamera->GetViewMatrix() * model;
		GLint mvpId = glGetProgramResourceLocation(program, GL_UNIFORM, "MVP");
		glProgramUniformMatrix4fv(program, mvpId, 1, GL_FALSE, glm::value_ptr(mvp));

		// Model
		GLint modelId = glGetProgramResourceLocation(program, GL_UNIFORM, "model");
		glProgramUniformMatrix4fv(program, modelId, 1, GL_FALSE, glm::value_ptr(model));

		// View	
		GLint viewId = glGetProgramResourceLocation(program, GL_UNIFORM, "view");
		glProgramUniformMatrix4fv(program, viewId, 1, GL_FALSE, glm::value_ptr(mainCamera->GetViewMatrix()));
		
		// View Model
		GLint viewModelId = glGetProgramResourceLocation(program, GL_UNIFORM, "viewModel");
		glm::mat4 viewModel = mainCamera->GetViewMatrix() * model;
		glProgramUniformMatrix4fv(program, viewModelId, 1, GL_FALSE, glm::value_ptr(viewModel));

		// Normal Matrix
		GLint normalId = glGetProgramResourceLocation(program, GL_UNIFORM, "normalMatrix");
		glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(viewModel));
		glProgramUniformMatrix3fv(program, normalId, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		// Time
		GLint timeId = glGetProgramResourceLocation(program, GL_UNIFORM, "time");
		glProgramUniform1f(program, timeId, time);

		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, mesh.positions.size());

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

		display(mesh.positions, mvp);
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
	glEnable(GL_CULL_FACE); // Por defeito está desativado
	glCullFace(GL_BACK);
}

void display(std::vector<glm::vec3> obj, glm::mat4 mvp) {
	

}



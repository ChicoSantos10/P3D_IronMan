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
#include <glm/gtc/type_ptr.hpp> 

#include "Camera.h"
#include "Shader.h"
#include "Model.h"

#define WIDTH 800
#define HEIGHT 600
#define MSAA_SAMPLES 4

// Functions
void initialize();
void display(std::vector<glm::vec3> obj, glm::mat4 mvp);

std::vector<glm::vec3> Load3DModel(void) {
	// 6 faces * 4 v�rtices por face
	glm::vec3 point[6 * 4] = {
		// Frente
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		// Tr�s
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		// Direita
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		// Esquerda
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		// Cima
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		// Baixo
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f,  1.0f)
	};

	std::vector<glm::vec3> ret;
	for (auto i : point)
		ret.push_back(i);

	return ret;
}

// Variables
Camera *mainCamera;
double deltaTime;
double prevTime = 0;

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
	std::vector<glm::vec3> obj = Load3DModel();

	GLFWwindow* window;

	// TODO: pass the model position as target
	glm::vec3 target(0.0f, 0.0f, -1.0f);
	mainCamera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), target, float(WIDTH) / float(HEIGHT));
	
	if (!glfwInit())
		return -1;

	// Sets the sample for the multi sample anti aliasing
	glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLES);

	window = glfwCreateWindow(WIDTH, HEIGHT, "P3D: Iron Man", NULL, NULL);

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

	// VAO
	glGenVertexArrays(1, VAOs);
	glBindVertexArray(VAOs[0]);

	// VBOs
	glGenBuffers(1, buffers);

	// Position buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.positions.size(), mesh.positions.data(), 0);
	GLint pos = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vpos");
	GLint bindId = 0;
	glVertexAttribFormat(pos, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(pos, bindId);
	glBindVertexBuffer(bindId, buffers[0], 0, sizeof(GLfloat) * 3);
	//glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(pos);

	// Uv buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvs.size(), mesh.uvs.data(), 0);
	GLint uvId = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "texcoord");
	bindId = 1;
	glVertexAttribFormat(uvId, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(uvId, bindId);
	glBindVertexBuffer(bindId, buffers[1], 0, sizeof(GLfloat) * 2);
	//glVertexAttribPointer(uvId, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(uvId);

	// Normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), 0);
	GLint normalId = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "normal");
	bindId = 2;
	glVertexAttribFormat(normalId, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(normalId, bindId);
	glBindVertexBuffer(bindId, buffers[2], 0, sizeof(GLfloat) * 3);
	//glVertexAttribPointer(normalId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(normalId);

    glm::mat4 translation(1.0f);
	glm::mat4 rotation(1.0f);
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//// TODO: Model class 
		//target += glm::vec3(0.0f, 0.001f, 0.0f);
		//translation = glm::translate(glm::mat4(1.0f), target);
		//rotation = glm::rotate(glm::mat4(1.0f), ANGLE += 0.01f, glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
		//glm::mat4 model = translation * rotation;
		//// Vai efetuando uma rota��o ao objeto (apenas para podermos ver todas as faces desenhadas).
		//model = glm::rotate(model, ANGLE += 0.001f, glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
		//model = glm::translate(model, target);

		// Updating delta time
		double time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		// MVP
		glm::mat4 mvp = mainCamera->GetProjectionMatrix() * mainCamera->GetViewMatrix() * model;

		GLint mvpId = glGetProgramResourceLocation(program, GL_UNIFORM, "MVP");
		glProgramUniformMatrix4fv(program, mvpId, 1, GL_FALSE, glm::value_ptr(mvp));

		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, mesh.positions.size());

		// Rotation based on mouse position
		if (glfwGetWindowAttrib(window, GLFW_HOVERED))
		{
			// Areas where the cursor needs to be to rotate
			const int threshold = WIDTH * 0.2f;
			const GLfloat MAX_ROTATION = glm::radians(90.0f) * deltaTime;
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			if (mouseX < threshold)
				mainCamera->Rotate((threshold - mouseX) / threshold * MAX_ROTATION);
			else if (mouseX > WIDTH - (double)threshold)
				mainCamera->Rotate((WIDTH - (double)threshold - mouseX) / threshold * MAX_ROTATION);
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
	//glEnable(GL_CULL_FACE);
}

void display(std::vector<glm::vec3> obj, glm::mat4 mvp) {
	

	//float* vertex_stream = static_cast<float*>(glm::value_ptr(obj.front()));

	//std::vector<glm::vec3> colors{
	//	glm::vec3(1.0f, 0.0f, 0.0f), // Red
	//	glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
	//	glm::vec3(0.0f, 1.0f, 0.0f), // Green
	//	glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
	//	glm::vec3(0.0f, 0.0f, 1.0f), // Blue
	//	glm::vec3(1.0f, 0.0f, 1.0f)  // Magenta
	//};

	// Desenha quad em modo imediato
	//glBegin(GL_QUADS);
	///* obj.size() * (obj.front().length()) � o mesmo que (6*4)*3 */
	///* 6 faces * 4 v�rtices por face * 3 coordenadas por v�rtice */
	//for (int nv = 0; nv < 6 * 4 * 3; nv += 3) {
	//	// Uma cor por face
	//	glColor3f(colors[nv / (4 * 3)].r, colors[nv / (4 * 3)].g, colors[nv / (4 * 3)].b);
	//	glm::vec4 vertex = glm::vec4(vertex_stream[nv], vertex_stream[nv + 1], vertex_stream[nv + 2], 1.0f);
	//	// C�lculo das coordenadas de recorte
	//	glm::vec4 transformed_vertex = mvp * vertex;
	//	// Divis�o de Perspetiva
	//	glm::vec4 normalized_vertex = transformed_vertex / transformed_vertex.w;
	//	// Desenho do v�rtice
	//	glVertex3f(normalized_vertex.x, normalized_vertex.y, normalized_vertex.z);
	//}
	//glEnd();
}



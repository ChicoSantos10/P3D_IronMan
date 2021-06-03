#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <glm\gtc\matrix_inverse.hpp> // glm::inverseTranspose()

#include "Camera.h"
#include "Shader.h"
#include "Model.h"

#define WIDTH 800
#define HEIGHT 600
#define MSAA_SAMPLES 4

//Variveis para ver se a luz ja está ativada 
struct LightSelecter {
	bool ambientLightSelected = false;
	bool directionalLightSelected = false;
	bool pointlightSelected = false;
	bool point2LightSelected = false;
	bool ambientLightwasActive = false;
	bool directionalLightwasActive = false;
	bool pointLightwasActive = false;
	bool point2LightwasActive = false;
};
LightSelecter lightSelecter;

// Functions
void initialize();
void display(std::vector<glm::vec3> obj, glm::mat4 mvp);
void init();
void chooseLight(GLFWwindow* window);

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

glm::mat4 model(1.0f), view(1.0f), projection(1.0f), ModelView;
glm::mat3 normalMatrix;

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
		/*{ GL_VERTEX_SHADER,   "Deformation.vert" },
		{ GL_FRAGMENT_SHADER, "Color.frag" },*/
		{ GL_VERTEX_SHADER,   "light.vert", },
		{ GL_FRAGMENT_SHADER, "light.frag" },
		{ GL_NONE, NULL }
	};

	/*{ GL_VERTEX_SHADER,   "light.vert", },
	  { GL_FRAGMENT_SHADER, "light.frag" },*/

	program = LoadShaders(shaders);
	if (!program)
		exit(EXIT_FAILURE);
	glUseProgram(program);

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
	//glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(pos);

	// Uv buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvs.size(), mesh.uvs.data(), 0);
	GLint uvId = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vtexcoord");
	bindId = 1;
	glVertexAttribFormat(uvId, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(uvId, bindId);
	glBindVertexBuffer(bindId, buffers[1], 0, sizeof(GLfloat) * 2);
	//glVertexAttribPointer(uvId, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(uvId);

	// Normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), 0);
	GLint normalId = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vnormal");
	bindId = 2;
	glVertexAttribFormat(normalId, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(normalId, bindId);
	glBindVertexBuffer(bindId, buffers[2], 0, sizeof(GLfloat) * 3);
	//glVertexAttribPointer(normalId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(normalId);

	// Textures
	GLint texId = glGetProgramResourceLocation(program, GL_UNIFORM, "text");
	glProgramUniform1i(program, texId, 0);



	// Habitita o atributo com localização 'vtexcoord' para o VAO ativo.
	glEnableVertexAttribArray(uvId);
	// Habitita o atributo com localização 'coresId' para o VAO ativo.
	glEnableVertexAttribArray(pos);
	// Habitita o atributo com localização 'normalId' para o VAO ativo.
	glEnableVertexAttribArray(normalId);

	glm::mat4 translation(1.0f);
	glm::mat4 rotation(1.0f);

	//Atribui os valores model/view/projection/modelview/normalMatrix a ser usados nos uniform

	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	view = mainCamera->GetViewMatrix();
	projection = mainCamera->GetProjectionMatrix();
	ModelView = view * model;
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelView));



	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

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

		init();

		chooseLight(window); //escolher a luz

		//display(mesh.positions, mvp);
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

void init(void) {
	// ****************************************************
	// Uniforms
	// ****************************************************
	// Atribui valor ao uniform Model
	GLint modelId = glGetProgramResourceLocation(program, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(program, modelId, 1, GL_FALSE, glm::value_ptr(model));
	// Atribui valor ao uniform View
	GLint viewId = glGetProgramResourceLocation(program, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(program, viewId, 1, GL_FALSE, glm::value_ptr(view));
	// Atribui valor ao uniform ModelView
	GLint modelViewId = glGetProgramResourceLocation(program, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(program, modelViewId, 1, GL_FALSE, glm::value_ptr(ModelView));
	// Atribui valor ao uniform Projection
	GLint projectionId = glGetProgramResourceLocation(program, GL_UNIFORM, "Projection");
	glProgramUniformMatrix4fv(program, projectionId, 1, GL_FALSE, glm::value_ptr(projection));
	// Atribui valor ao uniform NormalMatrix
	GLint normalViewId = glGetProgramResourceLocation(program, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(program, normalViewId, 1, GL_FALSE, glm::value_ptr(normalMatrix));


	//Atribuir os valores a variaveis de cada luz
	// Fonte de luz ambiente global
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(10.0f, 0.2f, 0.2f)));


	// Fonte de luz direcional
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));


	// Fonte de luz pontual #1
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 5.0)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].constant"), 1.0f);
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].linear"), 0.06f);
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].quadratic"), 0.02f);

	// Fonte de luz pontual #2
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].position"), 1, glm::value_ptr(glm::vec3(-2.0, 2.0, 5.0)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].constant"), 1.0f);
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].linear"), 0.06f);
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].quadratic"), 0.02f);



	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void display(std::vector<glm::vec3> obj, glm::mat4 mvp) {
	// Limpa o buffer de profundidade
	glClear(GL_DEPTH_BUFFER_BIT);
	///
	/// Atualiza os dados do Uniform
	///
	ModelView = projection * view * model;
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelView));
	// Atribui valor ao uniform Model
	GLint modelId = glGetProgramResourceLocation(program, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(program, modelId, 1, GL_FALSE, glm::value_ptr(model));
	GLint viewId = glGetProgramResourceLocation(program, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(program, viewId, 1, GL_FALSE, glm::value_ptr(view));
	GLint modelViewId = glGetProgramResourceLocation(program, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(program, modelViewId, 1, GL_FALSE, glm::value_ptr(ModelView));
	// Atribui valor ao uniform NormalMatrix
	GLint normalViewId = glGetProgramResourceLocation(program, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(program, normalViewId, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	// Vincula (torna ativo) o VAO
	glBindVertexArray(VAOs[0]);

}

void chooseLight(GLFWwindow* window) {

	//ambient light button 1
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !lightSelecter.ambientLightwasActive) {
		lightSelecter.ambientLightSelected = true;
		lightSelecter.ambientLightwasActive = true;
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "lightSelecter.ambientLightSelected"), lightSelecter.ambientLightSelected);
		printf("Escolheu a luz ambient");
	}
	if (glfwGetKey(window, GLFW_KEY_1) != GLFW_PRESS && lightSelecter.ambientLightwasActive) { //Deasativa a luz
		lightSelecter.ambientLightSelected = false;
		lightSelecter.ambientLightwasActive = false;
	}

	//direction light button 2
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !lightSelecter.directionalLightwasActive) {
		lightSelecter.directionalLightSelected = true;
		lightSelecter.directionalLightwasActive = true;
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "lightSelecter.directionalLightSelected"), lightSelecter.directionalLightSelected);
		printf("Escolheu a luz direional");
	}
	else if (glfwGetKey(window, GLFW_KEY_2) != GLFW_PRESS && lightSelecter.directionalLightwasActive) { //Deasativa a luz
		lightSelecter.directionalLightSelected = false;
		lightSelecter.directionalLightwasActive = false;
	}

	//point light button 3
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !lightSelecter.pointLightwasActive) {
		lightSelecter.pointlightSelected = true;
		lightSelecter.pointLightwasActive = true;
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "lightSelecter.pointlightSelected"), lightSelecter.pointlightSelected);
		printf("Escolheu a luz pontual");
	}
	else if (glfwGetKey(window, GLFW_KEY_3) != GLFW_PRESS && lightSelecter.pointLightwasActive) { //Deasativa a luz
		lightSelecter.pointlightSelected = false;
		lightSelecter.pointLightwasActive = false;
	}

	//conic light button 4
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !lightSelecter.point2LightwasActive) {
		lightSelecter.point2LightSelected = true;
		lightSelecter.point2LightwasActive = true;
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "lightSelecter.point2LightSelected"), lightSelecter.point2LightSelected);
		printf("Escolheu a luz conica");
	}
	else if (glfwGetKey(window, GLFW_KEY_4) != GLFW_PRESS && lightSelecter.point2LightwasActive) { //Deasativa a luz
		lightSelecter.point2LightSelected = false;
		lightSelecter.point2LightwasActive = false;
	}
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <terrain.h>
#include <gui.h>
#include <GameController.h>
#include <GameObject.h>

#include <iostream>

using namespace KooNan;
using namespace glm;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------

	GLFWwindow* window = glfwCreateWindow(GameController::SCR_WIDTH, GameController::SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	GameController::initGameController(window);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// Terrain
	// ------------------------------------
	Shader terrainShader("landscape/terrain.vs", "landscape/terrain.fs");

	unsigned int texture3;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(FileSystem::getPath("gui/resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	data = stbi_load(FileSystem::getPath("gui/resources/textures/grass.png").c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	vector<Texture> textures;
	struct Texture terrain_text{ texture3 , "texture_diffuse", FileSystem::getPath("gui/resources/textures/grass.png").c_str()};
	textures.push_back(terrain_text);
	Terrain smooth_ground(0, 0, textures);


	// Model
	// ------------------------------------
	Shader ourShader("model/model.vs", "model/model.fs");
	Model* planet = new Model(FileSystem::getPath("model/rsc/planet/planet.obj"));

	// Object
	// ------------------------------------
	GameObject* p1 = new GameObject(string("model/rsc/planet/planet.obj"),
		scale(translate(mat4(1.0f), vec3(0.0f, 5.0f, 0.0f)), vec3(0.5f, 0.5f, 0.5f)));
	GameObject* p2 = new GameObject(string("model/rsc/planet/planet.obj"),
		scale(translate(mat4(1.0f), vec3(5.0f, 2.0f, 0.0f)), vec3(0.5f, 0.5f, 0.5f)));

	// GUI
	// ------------------------------------
	GUI::initEnv(window);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		GameController::updateGameController(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(GameController::mainCamera.Zoom),
			(float)GameController::SCR_WIDTH / (float)GameController::SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = GameController::mainCamera.GetViewMatrix();

		terrainShader.use();
		terrainShader.setMat4("projection", projection);
		terrainShader.setMat4("view", view);
		terrainShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		smooth_ground.Draw(terrainShader);

		for (GameObject* obj : GameObject::gameObjList)
			obj->Draw(ourShader, projection, view);

		GUI::newFrame();
		GUI::drawWidgets();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// GameObject clear
	for (GameObject* obj : GameObject::gameObjList)
		delete obj;
	GameObject::gameObjList.clear();
	unordered_map<string, Model*>::iterator itr;
	for (itr = Model::modelList.begin(); itr != Model::modelList.end(); ++itr)
		delete itr->second;
	Model::modelList.clear();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
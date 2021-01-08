#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GameController.h>
#include <terrain.h>
#include <gui.h>
#include <GameController.h>
#include <GameObject.h>
#include <common.h>
#include <waterframebuffer.h>
#include <pickingtexture.h>
#include <scene.h>
#include <cube.h>
#include <gui.h>
#include <light.h>
#include <Texture.h>
#include <iostream>

using namespace KooNan;
using namespace glm;

//Define four point lights position
glm::vec3 pointLightPositions[] = {
		glm::vec3(2.0f,  3.0f,  2.0f),
		glm::vec3(2.3f, 2.0f, -5.0f),
		glm::vec3(-4.0f,  2.5f, -8.0f),
		glm::vec3(0.0f,  1.5f, -3.0f)
};//This should match up with the macro NR_POINT_LIGHTS in fragment shader

// Define positions of cubes
glm::vec3 cubePositions[] = {
		glm::vec3(-9.0f,  0.0f,  0.0f),
		glm::vec3(-10.0f,  0.0f, 0.0f),
		glm::vec3(-9.0f, 0.0f, -3.0f),
		glm::vec3(-10.5f, 0.0f, 0.0f),
		glm::vec3(-9.0f, 0.0f, -3.7f),
		glm::vec3(-10.5f, 0.0f, 0.0f),
		glm::vec3(-9.0f, 0.0f, -4.7f),
		glm::vec3(-11.0f, 0.0f, 0.0f),
		glm::vec3(-15.0f, 0.0f, -6.7f),
		glm::vec3(-16.0f, 0.0f, 0.0f),
};

// Define paths of sky box textures
std::vector<std::string> skyboxPaths = {
		FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_px.jpg").c_str(),
		FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_nx.jpg").c_str(),
		FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_py.jpg").c_str(),
		FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_ny.jpg").c_str(),
		FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_pz.jpg").c_str(),
		FileSystem::getPath("landscape/resources/textures/skybox/TropicalSunnyDay_nz.jpg").c_str(),
};

// Define paths of ground and water textures
//Format:(1).groundheightmap (2~size-2).groundtexture (size-1).waterdudvmap (size).waternormalmap
std::vector<std::string> groundPaths = {
	FileSystem::getPath("landscape/resources/textures/heightmap.png"),
	FileSystem::getPath("landscape/resources/textures/blendermap.jpg"),
	FileSystem::getPath("landscape/resources/textures/background1.jpg"),
	FileSystem::getPath("landscape/resources/textures/groundR.jpg"),
	FileSystem::getPath("landscape/resources/textures/groundG.png"),
	FileSystem::getPath("landscape/resources/textures/groundB.jpg"),
	FileSystem::getPath("landscape/resources/textures/waterDUDV.png"),
	FileSystem::getPath("landscape/resources/textures/normal.png")
};
void addlights(Light& light);


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------

    GLFWwindow* window = glfwCreateWindow(Common::SCR_WIDTH, Common::SCR_HEIGHT, "Koonan", NULL, NULL);
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



	// configure global opengl state
	// -----------------------------
	glEnable(GL_MULTISAMPLE);

    // build and compile our shader program
    // ------------------------------------
    Shader boxShader(FileSystem::getPath("model/camera.vs").c_str(), FileSystem::getPath("model/camera.fs").c_str());
	Shader terrainShader(FileSystem::getPath("landscape/terrain.vs").c_str(), FileSystem::getPath("landscape/terrain.fs").c_str());
	Shader waterShader(FileSystem::getPath("landscape/water.vs").c_str(), FileSystem::getPath("landscape/water.fs").c_str());
	Shader skyShader(FileSystem::getPath("landscape/skybox.vs").c_str(), FileSystem::getPath("landscape/skybox.fs").c_str());
	Shader lightShader(FileSystem::getPath("landscape/lightcube.vs").c_str(), FileSystem::getPath("landscape/lightcube.fs").c_str());
	Shader pickingShader(FileSystem::getPath("gui/picking.vs").c_str(), FileSystem::getPath("gui/picking.fs").c_str());
	Shader modelShader("model/model.vs", "model/model.fs");

    
   
	// Instantiate the main_scene
	Scene main_scene(256.0f, 1, 1, -0.7f, terrainShader, waterShader, skyShader, groundPaths, skyboxPaths);

	// Instantiate the light(with only "parallel" light component)
	// ------------------------------------
	DirLight parallel{
		glm::vec3(0.3f, -0.7f, 1.0f),
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.7f, 0.7f, 0.7f),
		glm::vec3(0.4f, 0.4f, 0.4f)
	};
	Light main_light(parallel, lightShader);
	addlights(main_light);// Add four point lights



	// Set the texture and material for boxes
	TextureManager texman(GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);// Instantiation the texture loader class for our boxes
	vector<Texture>      box_textures;//Initialize an empty texture vector
	box_textures.push_back(texman.LoadTexture(FileSystem::getPath("gui/resources/textures/container.jpg"),"texture_diffuse"));//Push back the loaded texture by "LoadTexture"
	Cube boxes(box_textures, boxShader);// Instantiate the boxes
	boxShader.use();
	boxShader.setInt("material.diffuse", 0);
	boxShader.setInt("material.specular", 0);
	boxShader.setFloat("material.shininess", 32.0f);

	// Set lights for scene and entities using "SetLight(Shader&)"
	// -------------------------------------------------------------------------------------------
	terrainShader.use();
	main_light.SetLight(terrainShader);

	waterShader.use();
	main_light.SetLight(waterShader);

	boxShader.use();
	main_light.SetLight(boxShader);


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
	// Model
	// ------------------------------------
	
	Model* planet = new Model("model/rsc/planet/planet.obj");

	// Object
	// ------------------------------------
	GameObject* p1 = new GameObject(string("model/rsc/planet/planet.obj"),
		scale(translate(mat4(1.0f), vec3(0.0f, 5.0f, 0.0f)), vec3(0.5f, 0.5f, 0.5f)));
	GameObject* p2 = new GameObject(string("model/rsc/planet/planet.obj"),
		scale(translate(mat4(1.0f), vec3(5.0f, 2.0f, 0.0f)), vec3(0.5f, 0.5f, 0.5f)));

	// GUI
	// ------------------------------------
	GUI::initEnv(window);
	GUI::updateModelTextures(modelShader);

	PickingTexture mouse_picking;
	Water_Frame_Buffer waterfb;
	
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
        // per-frame time logic
        // --------------------
		GameController::updateGameController(window);


		//需要渲染三次 前两次不渲染水面 最后一次渲染水面

		//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
		//--------------------------------------------------------------------------------------------------------------
		//FIRST TIME RENDERING TO REFLECTION
		//--------------------------------------------------------------------------------------------------------------
		//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

		glEnable(GL_CLIP_DISTANCE0);
		waterfb.bindReflectionFrameBuffer();
		glEnable(GL_DEPTH_TEST);
        // render
        // ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
        // Set the main camera to the position and direction of reflection
		float distance = 2 * (GameController::mainCamera.Position.y - main_scene.getWaterHeight());
		GameController::mainCamera.Position.y -= distance;
		GameController::mainCamera.Pitch = -GameController::mainCamera.Pitch;
 

        // render boxes

        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			model = glm::translate(model, glm::vec3(cubePositions[i].x,main_scene.getTerrainHeight(cubePositions[i].x, cubePositions[i].z), cubePositions[i].z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			boxes.Draw(GameController::mainCamera, glm::vec4(0.0, 1.0, 0.0, -main_scene.getWaterHeight()), model);
        }
		p1->Draw(modelShader, Common::GetPerspectiveMat(GameController::mainCamera), GameController::mainCamera.GetViewMatrix());

		// we now draw as many light bulbs as we have point lights.
		main_light.Draw(GameController::mainCamera, glm::vec4(0.0, 1.0, 0.0, -main_scene.getWaterHeight()));
		//render the main scene
		main_scene.Draw(GameController::deltaTime, GameController::mainCamera, glm::vec4(0.0, 1.0, 0.0, -main_scene.getWaterHeight()), false);

		/*
		Render the else you need to render here!! Remember to set the clipping plane!!!
		*/

		//Restore the main camera
		GameController::mainCamera.Position.y += distance;
		GameController::mainCamera.Pitch = -GameController::mainCamera.Pitch;
		GameController::mainCamera.GetViewMatrix();

		waterfb.unbindCurrentFrameBuffer();

		unsigned int reflect_text = waterfb.getReflectionTexture();
		

		//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
		//--------------------------------------------------------------------------------------------------------------
		//SECOND TIME RENDERING TO REFRACTION
		//--------------------------------------------------------------------------------------------------------------
		//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

		waterfb.bindRefractionFrameBuffer();
		glEnable(GL_DEPTH_TEST);
		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			model = glm::translate(model, glm::vec3(cubePositions[i].x, main_scene.getTerrainHeight(cubePositions[i].x, cubePositions[i].z), cubePositions[i].z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			boxes.Draw(GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, main_scene.getWaterHeight()), model);
		}
		// we now draw as many light bulbs as we have point lights.

		p1->Draw(modelShader, Common::GetPerspectiveMat(GameController::mainCamera), GameController::mainCamera.GetViewMatrix());
		main_light.Draw(GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, main_scene.getWaterHeight()));
		main_scene.Draw(GameController::deltaTime, GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, main_scene.getWaterHeight()), false);

		/*
		Render the else you need to render here!! Remember to set the clipping plane!!!
		*/

		unsigned int refract_text = waterfb.getRefractionTexture(); 



		//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
		//--------------------------------------------------------------------------------------------------------------
		//THIRD TIME RENDERING TO SCREEN
		//--------------------------------------------------------------------------------------------------------------
		//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

		waterfb.unbindCurrentFrameBuffer();

		//Record picking info into the framebuffer
		//--------------------------------------------
		mouse_picking.bindFrameBuffer();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			glm::vec3 model_position = glm::vec3(cubePositions[i].x, main_scene.getTerrainHeight(cubePositions[i].x, cubePositions[i].z) + 0.15f, cubePositions[i].z);
			model = glm::translate(model, model_position);
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			boxes.Pick(pickingShader, GameController::mainCamera, model, i + 1, 0);

		}


		mouse_picking.unbindFrameBuffer();




		
		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// pass projection matrix to shader (note that in this case it could change every frame)
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			glm::vec3 model_position = glm::vec3(cubePositions[i].x, main_scene.getTerrainHeight(cubePositions[i].x, cubePositions[i].z) + 0.15f, cubePositions[i].z);
			model = glm::translate(model, model_position);
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			float hitObjID = mouse_picking.ReadPixel(GameController::cursorX, Common::SCR_HEIGHT - GameController::cursorY - 22).ObjID;//deviation of y under resolution 1920*1080 maybe 22

			bool intersected = false;
			if ((unsigned int)hitObjID == i + 1)
			{
				intersected = true;
			}
			boxes.Draw(GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, 99999.0f), model, intersected);
		}
		p1->Draw(modelShader, Common::GetPerspectiveMat(GameController::mainCamera), GameController::mainCamera.GetViewMatrix());
		main_light.Draw(GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, 99999.0f));


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		main_scene.setReflectText(reflect_text);
		main_scene.setRefractText(refract_text);
		main_scene.setDepthMap(waterfb.getRefractionDepthTexture());
		main_scene.Draw(GameController::deltaTime, GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, 99999.0f), true);

		
		glDisable(GL_BLEND);
		
		/*
		Render the else you need to render here!! Remember to set the clipping plane!!!
		*/
		
		glDisable(GL_DEPTH_TEST);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
		//for (GameObject* obj : GameObject::gameObjList)
		//	obj->Draw(ourShader, projection, view);

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
	waterfb.cleanUp();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
void addlights(Light& light)
{
	
	for (int i = 0; i < 4; i++)
	{
		PointLight l
		{
			glm::vec3(pointLightPositions[i]),
			1.0f,
			0.09f,
			0.032f,
			glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			glm::vec3(1.0f, 1.0f, 1.0f),
		};
		light.AddPointLight(l);
	}

}



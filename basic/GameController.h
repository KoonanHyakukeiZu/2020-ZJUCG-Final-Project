#pragma once
#include <Camera.h>
#include <model.h>
#include <scene.h>
#include <light.h>
#include <VideoRecord.h>

#include <glm/glm.hpp>
#define GLM_SWIZZLE
#include <GLFW/glfw3.h>

#include <common.h>
#include <GameObject.h>
#include <mousepicker.h>

#include <unordered_map>
#include <fstream>
#include <json.hpp>
using nlohmann::json;

#define SAVEJSON_GAMEOBJLIST "GameObjectList"
#define SAVEJSON_GAMEOBJ_PATH "path"
#define SAVEJSON_GAMEOBJ_MAT "mat"
#define SAVEJSON_GAMEOBJ_PICKABLE "pickable"
#define SAVEJSON_GAMEOBJ_SCALE "scale"
#define SAVEJSON_GAMEOBJ_POSITION "position"
#define SAVEJSON_GAMEOBJ_ROTY "rotateY"
#define SAVEJSON_POINTLIGHTLIST "PointLightList"
#define SAVEJSON_PARALLELLIGHT "ParallelLight"
#define SAVEJSON_LIGHT_POSITION "position"
#define SAVEJSON_LIGHT_DIRECTION "direction"
#define SAVEJSON_LIGHT_AMBIENT "ambient"
#define SAVEJSON_LIGHT_DIFFUSE "diffuse"
#define SAVEJSON_LIGHT_SPECULAR "specular"
#define SAVEJSON_LIGHT_CONSTANT "constant"
#define SAVEJSON_LIGHT_LINEAR "linear"
#define SAVEJSON_LIGHT_QUADRATIC "quadratic"

namespace KooNan
{
	enum class GameMode
	{// 第一级状态
		Title, // 标题页
		Pause, // 暂停页
		Wandering, // 漫步游览模式
		Creating // 创造模式
	};
	enum class CreatingMode
	{
		Placing, // 放置模式
		Selecting, // 选择模式
		Editing, // 编辑模型模式
		EditingLight, // 编辑光照
	};
	enum class MouseMode {
		GUIMode, CameraMode
	};
	class GameController
	{
		// 全局状态
	public:
		static MouseMode mouseMode;
		static float deltaTime;
		static float lastFrame;

		static double cursorX, cursorY;
		static double lastCursorX, lastCursorY;

		static Camera mainCamera;
		static Camera oriCreatingCamera;

		static Scene* mainScene;
		static Light* mainLight;
		
		static string selectedModel; // 当前选择的模组
		static GameObject* helperGameObj; // 辅助游戏物体
		static GameObject* selectedGameObj; // 光标移动过程中选中的游戏物体

		static MousePicker mousePicker;
		static int RECURSION_COUNT;

		// 全局信号：由GUI模块或键鼠输入写入，被其他模块读取
	public:
		static GameMode gameMode; // 游戏模式
		static GameMode lastGameMode;
		static CreatingMode creatingMode; // 创造模式子模式
		static bool isRecordingLast; // 上一次循环是否正在录制
		static bool isRecording; // 是否正在录制
		static Model::ModelType modelType; // 模型列表的类型
		static bool isCursorOnGui;
		// 常量
	public:
		const static unsigned int EDGE_WIDTH = 50;
		// 状态
	private:
		static bool firstMouse; // 是否是第一次点击（用于鼠标移动事件）
		static bool ctrlPressedLast; // 上一次循环是否按下ctrl键
		static bool altPressedLast; // 上一次循环是否按下alt键
		static bool midBtnPressedLast; // 上一次循环是否按下鼠标中键
	public:
		static void initGameController(GLFWwindow* window)
		{
			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, GameController::framebuffer_size_callback);
			glfwSetCursorPosCallback(window, GameController::cursor_callback);
			glfwSetScrollCallback(window, GameController::scroll_callback);
		}
		static void updateGameController(GLFWwindow* window)
		{
			float currentFrame = glfwGetTime();
			GameController::deltaTime = currentFrame - GameController::lastFrame;
			GameController::lastFrame = currentFrame;

			glfwGetCursorPos(window, &cursorX, &cursorY);

			GameController::processInput(window);

			// 创造模式下使用鼠标可以移动相机
			if (gameMode == GameMode::Creating && !isCursorOnGui) {

				if (cursorX <= EDGE_WIDTH)
					mainCamera.ProcessKeyboard(deltaTime, WEST);
				else if (cursorX >= Common::SCR_WIDTH - EDGE_WIDTH)
					mainCamera.ProcessKeyboard(deltaTime, EAST);
				if (cursorY <= EDGE_WIDTH)
					mainCamera.ProcessKeyboard(deltaTime, NORTH);
				else if (cursorY >= Common::SCR_HEIGHT - EDGE_WIDTH)
					mainCamera.ProcessKeyboard(deltaTime, SOUTH);
			}

			// 检测相机是否低于地形，纠正
			if (mainScene)
			{
				static float border = 0.5f;
				try
				{
					float h = mainScene->getTerrainHeight(mainCamera.Position.x, mainCamera.Position.z);
					h = mainScene->getWaterHeight() > h ? mainScene->getWaterHeight() : h;

					if (gameMode == GameMode::Creating && h >= mainCamera.Position.y)
						mainCamera.Position.y = h + border;
					else if (gameMode == GameMode::Wandering)
						mainCamera.Position.y = h + border;
				}
				catch (const char* msg)
				{
					float h = mainScene->getTerrainHeight(mainCamera.Position.x, mainCamera.Position.z);
					h = mainScene->getWaterHeight() > h ? mainScene->getWaterHeight() : h;
					mainCamera = oriCreatingCamera;
					if (gameMode == GameMode::Wandering)
						mainCamera.Position.y = h + border;
				}
			}

			// 光标移动时，若有模型选中，显示辅助物体
			if (gameMode == GameMode::Creating && creatingMode == CreatingMode::Placing && !isCursorOnGui)
			{
				glm::vec3 t = findFocusInScene();
				if (t != mainCamera.Position)
				{
					if (selectedModel != "" && helperGameObj == NULL)
					{// 新建
						glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), t);
						helperGameObj = new GameObject(selectedModel.c_str(), modelMat, true);
						selectedModel = "";
					}
					else if (helperGameObj)
					{// 移动
						helperGameObj->pos = t; 
						helperGameObj->Update();
					}
				}
			}

			if (!isRecordingLast && isRecording)
			{// 开始录像
				isRecordingLast = isRecording;
				VideoRecord::RecordInit(60, Common::SCR_WIDTH, Common::SCR_HEIGHT);
			}
			else if (isRecordingLast && !isRecording)
			{// 结束录像
				isRecordingLast = isRecording;
				VideoRecord::EndRecord();
			}
			else if (isRecordingLast && isRecording)
			{// 录像
				VideoRecord::GrabFrame();
			}
				
		}
		static void changeGameModeTo(GameMode newmode) 
		{
			lastGameMode = gameMode;
			gameMode = newmode;

			if (gameMode == GameMode::Wandering) {
				altPressedLast = true;
			}
			else if (gameMode == GameMode::Creating) 
			{
				GameController::mainCamera = GameController::oriCreatingCamera;
				GameController::creatingMode = CreatingMode::Selecting;
			}
		}
		static void revertGameMode() 
		{
			changeGameModeTo(lastGameMode);
		}
		static void addLightToMainLight(const PointLight& pl) 
		{
			if (mainLight == NULL)return;
			mainLight->AddPointLight(pl);
		}
		static bool LoadGameFromFile()
		try {
			ifstream fin;
			fin.open(Common::saveFileName);
			if (fin.is_open()) {
				json gameInfo;
				fin >> gameInfo;
				if (gameInfo.is_null()) return false;
				for (auto& it : gameInfo[SAVEJSON_GAMEOBJLIST]) {
					string modelPath = it[SAVEJSON_GAMEOBJ_PATH];
					bool isPickable = it[SAVEJSON_GAMEOBJ_PICKABLE] != 0;
					float rotY = it[SAVEJSON_GAMEOBJ_ROTY];
					glm::vec3 pos;
					for (int i = 0; i < 3; ++i) pos[i] = it[SAVEJSON_GAMEOBJ_POSITION][i];
					glm::vec3 scale;
					for (int i = 0; i < 3; ++i) scale[i] = it[SAVEJSON_GAMEOBJ_SCALE][i];
					glm::mat4 mat;
					for (int i = 0; i < 16; ++i) mat[i / 4][i % 4] = it[SAVEJSON_GAMEOBJ_MAT][i];
					new GameObject(modelPath, mat, isPickable, pos, rotY, scale);
				}
				// lighting
				// parallel light
				glm::vec3 pdir, pambient, pdiffuse, pspecular;
				for (int i = 0; i < 3; ++i) {
					pdir[i] = gameInfo[SAVEJSON_PARALLELLIGHT][SAVEJSON_LIGHT_DIRECTION][i];
					pambient[i] = gameInfo[SAVEJSON_PARALLELLIGHT][SAVEJSON_LIGHT_AMBIENT][i];
					pdiffuse[i] = gameInfo[SAVEJSON_PARALLELLIGHT][SAVEJSON_LIGHT_DIFFUSE][i];
					pspecular[i] = gameInfo[SAVEJSON_PARALLELLIGHT][SAVEJSON_LIGHT_SPECULAR][i];
				}
				DirLight* parallel = mainLight->getDirectionLight();
				parallel->direction = pdir;
				parallel->ambient = pambient;
				parallel->diffuse = pdiffuse;
				parallel->specular = pspecular;

				for (auto& it : gameInfo[SAVEJSON_POINTLIGHTLIST]) {
					glm::vec3 pos, ambient, diffuse, specular;
					for (int i = 0; i < 3; ++i) {
						pos[i] = it[SAVEJSON_LIGHT_POSITION][i];
						ambient[i] = it[SAVEJSON_LIGHT_AMBIENT][i];
						diffuse[i] = it[SAVEJSON_LIGHT_DIFFUSE][i];
						specular[i] = it[SAVEJSON_LIGHT_SPECULAR][i];
					}
					float constant, linear, quadratic;
					constant = it[SAVEJSON_LIGHT_CONSTANT];
					linear = it[SAVEJSON_LIGHT_LINEAR];
					quadratic = it[SAVEJSON_LIGHT_QUADRATIC];
					mainLight->AddPointLight(PointLight{ pos,constant,linear,quadratic,ambient,diffuse,specular });
				}

			}
			else {
				cout << "Failed to open save file!" << endl;
			}
			return true;
		}
		catch (...) {
			cout << "Failed to load from save file!" << endl;
			return false;
		}
		static void SaveGameToFile()
		{
			ofstream fout;
			fout.open(Common::saveFileName);
			if (fout.is_open()) {
				json gameInfo;
				gameInfo[SAVEJSON_GAMEOBJLIST] = json::array();
				for (GameObject* p : GameObject::gameObjList) {
					json gameobj;
					gameobj[SAVEJSON_GAMEOBJ_PATH] = p->modelPath;
					gameobj[SAVEJSON_GAMEOBJ_PICKABLE] = (int)(p->IsPickable);
					gameobj[SAVEJSON_GAMEOBJ_ROTY] = p->rotY;
					gameobj[SAVEJSON_GAMEOBJ_SCALE] = json::array();
					for (int i = 0; i < 3; ++i) gameobj[SAVEJSON_GAMEOBJ_SCALE].push_back(p->sca[i]);
					gameobj[SAVEJSON_GAMEOBJ_POSITION] = json::array();
					for (int i = 0; i < 3; ++i) gameobj[SAVEJSON_GAMEOBJ_POSITION].push_back(p->pos[i]);
					gameobj[SAVEJSON_GAMEOBJ_MAT] = json::array();
					for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) gameobj[SAVEJSON_GAMEOBJ_MAT].push_back(p->modelMat[i][j]);
					gameInfo[SAVEJSON_GAMEOBJLIST].push_back(gameobj);
				}
				// lighting
				// parallel
				json parallelLight;
				DirLight* dL = mainLight->getDirectionLight();
				parallelLight[SAVEJSON_LIGHT_DIRECTION] = json::array();
				for (int i = 0; i < 3; ++i) parallelLight[SAVEJSON_LIGHT_DIRECTION].push_back(dL->direction[i]);
				parallelLight[SAVEJSON_LIGHT_AMBIENT] = json::array();
				for (int i = 0; i < 3; ++i) parallelLight[SAVEJSON_LIGHT_AMBIENT].push_back(dL->ambient[i]);
				parallelLight[SAVEJSON_LIGHT_DIFFUSE] = json::array();
				for (int i = 0; i < 3; ++i) parallelLight[SAVEJSON_LIGHT_DIFFUSE].push_back(dL->diffuse[i]);
				parallelLight[SAVEJSON_LIGHT_SPECULAR] = json::array();
				for (int i = 0; i < 3; ++i) parallelLight[SAVEJSON_LIGHT_SPECULAR].push_back(dL->specular[i]);
				gameInfo[SAVEJSON_PARALLELLIGHT] = parallelLight;

				// point
				json pointLights = json::array();
				int npls = mainLight->numOfPointLight();
				for (int i = 0; i < npls; ++i) {
					json Light;
					PointLight* pl = mainLight->getPointLightAt(i);
					Light[SAVEJSON_LIGHT_POSITION] = json::array();
					for (int i = 0; i < 3; ++i) Light[SAVEJSON_LIGHT_POSITION].push_back(pl->position[i]);
					Light[SAVEJSON_LIGHT_AMBIENT] = json::array();
					for (int i = 0; i < 3; ++i) Light[SAVEJSON_LIGHT_AMBIENT].push_back(pl->ambient[i]);
					Light[SAVEJSON_LIGHT_DIFFUSE] = json::array();
					for (int i = 0; i < 3; ++i) Light[SAVEJSON_LIGHT_DIFFUSE].push_back(pl->diffuse[i]);
					Light[SAVEJSON_LIGHT_SPECULAR] = json::array();
					for (int i = 0; i < 3; ++i) Light[SAVEJSON_LIGHT_SPECULAR].push_back(pl->specular[i]);
					Light[SAVEJSON_LIGHT_CONSTANT] = pl->constant;
					Light[SAVEJSON_LIGHT_LINEAR] = pl->linear;
					Light[SAVEJSON_LIGHT_QUADRATIC] = pl->quadratic;
					
					pointLights.push_back(Light);
				}
				gameInfo[SAVEJSON_POINTLIGHTLIST] = pointLights;

				fout << gameInfo.dump(4);
				fout.close();
				cout << "Saving successfully done." << endl;
			}
			else cout << "Failed to open save file!" << endl;
			return;
		}
	private:
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void processInput(GLFWwindow* window);
		static void updateCursorMode(GLFWwindow* window);

		// 找到光标射线与地形的交点
		static glm::vec3 findFocusInScene();
		static glm::vec3 binarySearch(int count, float start, float finish, glm::vec3 ray);
		static bool intersectionInRange(float start, float finish, glm::vec3 ray);
	};

	// 状态与信号初始化
	MouseMode GameController::mouseMode = MouseMode::GUIMode;
	float GameController::lastFrame = .0f;
	float GameController::deltaTime = .0f;
	double GameController::cursorX = .0;
	double GameController::cursorY = .0;
	double GameController::lastCursorX = .0;
	double GameController::lastCursorY = .0;

	Camera GameController::oriCreatingCamera = Camera(0.f, 10.0f, 40.f, 0.f, 0.f, 30.f, 0.f, 1.f, 0.f);
	Camera GameController::mainCamera = GameController::oriCreatingCamera;

	MousePicker GameController::mousePicker = MousePicker(GameController::mainCamera);

	GameMode GameController::gameMode = GameMode::Creating;
	GameMode GameController::lastGameMode = GameMode::Title;
	CreatingMode GameController::creatingMode = CreatingMode::Selecting;

	bool GameController::isRecordingLast = false;
	bool GameController::isRecording = false;

	Model::ModelType GameController::modelType = Model::ModelType::ComplexModel;

	bool GameController::isCursorOnGui = false;

	bool GameController::firstMouse = true;
	bool GameController::ctrlPressedLast = false;
	bool GameController::altPressedLast = false;
	bool GameController::midBtnPressedLast = false;

	Scene* GameController::mainScene = NULL;
	Light* GameController::mainLight = NULL;

	string GameController::selectedModel = "";
	GameObject* GameController::helperGameObj = NULL;
	GameObject* GameController::selectedGameObj = NULL;

	int GameController::RECURSION_COUNT = 64;

	// 函数定义
	void GameController::framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		Common::SCR_HEIGHT = height;
		Common::SCR_WIDTH = width;
	}

	void GameController::cursor_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (mouseMode == MouseMode::GUIMode) return;

		static float lastX = Common::SCR_WIDTH / 2.0f;
		static float lastY = Common::SCR_HEIGHT / 2.0f;
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		// 避免光标不显示时点到GUI上的按钮，保持光标位置在屏幕中心
		lastX = Common::SCR_WIDTH / 2.0f;
		lastY = Common::SCR_HEIGHT / 2.0f;
		glfwSetCursorPos(window, lastX, lastY);

		if (gameMode == GameMode::Wandering) {
			mainCamera.ProcessMouseMovement(xoffset, yoffset);
		}
		else if (gameMode == GameMode::Creating) {
			if (creatingMode == CreatingMode::Editing);
			else
			{
				static float viewDist = 5.0f;
				mainCamera.ProcessMouseMovement(xoffset, yoffset, mainCamera.Position + viewDist * mainCamera.Front);
			}
		}
	}

	void GameController::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (gameMode == GameMode::Creating)
			if (creatingMode == CreatingMode::Placing)
				if (ctrlPressedLast)
					if (helperGameObj)
					{
						helperGameObj->rotY += glm::radians(yoffset);
					}
					else;
				else
					mainCamera.ProcessMouseScroll(HEIGHT_CHANGE, yoffset);
			else
				mainCamera.ProcessMouseScroll(HEIGHT_CHANGE, yoffset);
		else if(gameMode == GameMode::Wandering)
			mainCamera.ProcessMouseScroll(FOVY_CHANGE, yoffset);
	}

	void GameController::processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (gameMode == GameMode::Creating)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(deltaTime, NORTH);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(deltaTime, SOUTH);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(deltaTime, WEST);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(deltaTime, EAST);

			ctrlPressedLast = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

			bool midBtnPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
			if (midBtnPressed != midBtnPressedLast)
			{
				midBtnPressedLast = midBtnPressed;
				updateCursorMode(window);
			}

			if (creatingMode == CreatingMode::Placing)
			{
				static float scalStepWise = 1.1f;
				if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
					if (helperGameObj)
						helperGameObj->sca.z *= scalStepWise;
				if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
					if (helperGameObj)
						helperGameObj->sca.z /= scalStepWise;
				if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
					if (helperGameObj)
						helperGameObj->sca.x /= scalStepWise;
				if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
					if (helperGameObj)
						helperGameObj->sca.x *= scalStepWise;
				if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
					if (helperGameObj)
						helperGameObj->sca.y *= scalStepWise;
				if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
					if (helperGameObj)
						helperGameObj->sca.y /= scalStepWise;


				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
					if (helperGameObj) // 确定放置物体
						helperGameObj = NULL;
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
					if (helperGameObj) // 移除辅助物体
					{
						auto itr = GameObject::gameObjList.begin();
						for (; itr != GameObject::gameObjList.end(); ++itr)
							if (*itr == helperGameObj)
								break;
						GameObject::gameObjList.erase(itr);
						delete helperGameObj;
						helperGameObj = NULL;
					}
			}
			else if (creatingMode == CreatingMode::Selecting)
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
					if (selectedGameObj)
					{
						lastCursorX = cursorX;
						lastCursorY = cursorY;
						creatingMode = CreatingMode::Editing;
					}
		}
		else if (gameMode == GameMode::Wandering)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(deltaTime, FORWARD);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(deltaTime, BACKWARD);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(deltaTime, LEFT);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(deltaTime, RIGHT);

			bool altPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
			if (altPressed != altPressedLast)
			{
				altPressedLast = altPressed;
				updateCursorMode(window);
			}
		}
	}

	void GameController::updateCursorMode(GLFWwindow* window)
	{
		if (gameMode == GameMode::Wandering)
			if (altPressedLast)
			{
				mouseMode = MouseMode::GUIMode;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				GameController::firstMouse = true;
			}
			else
			{
				mouseMode = MouseMode::CameraMode;
				glfwSetCursorPos(window, Common::SCR_WIDTH / 2.0f, Common::SCR_HEIGHT / 2.0f);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		else if (gameMode == GameMode::Creating)
			if (midBtnPressedLast)
			{
				mouseMode = MouseMode::CameraMode;
				glfwSetCursorPos(window, Common::SCR_WIDTH / 2.0f, Common::SCR_HEIGHT / 2.0f);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else
			{
				mouseMode = MouseMode::GUIMode;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				GameController::firstMouse = true;
			}
	}
	glm::vec3 GameController::findFocusInScene()
	{
		if (mainScene == NULL)
			return mainCamera.Position;

		// 防止向上放置，防止getTerrainHeight的断言异常
		if (mainCamera.Front.y >= 0)
			return mainCamera.Position;

		mousePicker.update(cursorX, cursorY);
		glm::vec3 d3 = mousePicker.getCurrentRay();
		float MAX_PICKING_DISTANCE = 1000.0f;
		return GameController::binarySearch(0, 0, MAX_PICKING_DISTANCE, d3);
	}
	glm::vec3 GameController::binarySearch(int count, float start, float finish, glm::vec3 ray)
	{
		float half = start + ((finish - start) / 2.0f);
		if (count >= RECURSION_COUNT) {
			glm::vec3 endPoint = mainCamera.Position + ray*half;
			return glm::vec3(endPoint.x, mainScene->getTerrainHeight(endPoint.x, endPoint.z), endPoint.z);
		}
		if (GameController::intersectionInRange(start, half, ray)) {
			return binarySearch(count + 1, start, half, ray);
		}
		else {
			return binarySearch(count + 1, half, finish, ray);
		}
	}
	bool GameController::intersectionInRange(float start, float finish, glm::vec3 ray)
	{
		glm::vec3 startPoint = mainCamera.Position + ray*start;
		glm::vec3 endPoint = mainCamera.Position + ray*finish;
		float terrainHeightStart = mainScene->getTerrainHeight(startPoint.x, startPoint.z);
		float terrainHeightEnd = mainScene->getTerrainHeight(endPoint.x, endPoint.z);
		if (startPoint.y > terrainHeightStart&& endPoint.y < terrainHeightEnd)
			return true;
		else
			return false;
	}
}
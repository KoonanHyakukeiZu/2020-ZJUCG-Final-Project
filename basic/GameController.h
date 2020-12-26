#pragma once
#include <Camera.h>
#include <glm/glm.hpp>

namespace KooNan
{
	enum GameMode
	{
		Tile,
		Pause,
		Wandering,
		Creating
	};
	class GameController
	{
		// 全局状态
	public:
		static unsigned int SCR_WIDTH, SCR_HEIGHT;
		
		static bool firstMouse; // 是否是第一次点击（用于鼠标移动事件）
		static bool altPressedLast; // 上一次循环是否按下alt键
		static float deltaTime;
		static float lastFrame;

		static Camera mainCamera;

		// 全局信号：由GUI模块或键鼠输入写入，被其他模块读取
	public:
		static GameMode gameMode; // 游戏模式：暂停、漫游、创造
		static int sthSelected; // 场景中有物体被拾取
		// 键鼠输入处理，拾取
	public:
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void processInput(GLFWwindow* window);
		static void updateCursorMode(GLFWwindow* window);
	};

	// 状态与信号初始化
	unsigned int GameController::SCR_WIDTH = 1920;
	unsigned int GameController::SCR_HEIGHT = 1080;

	bool GameController::firstMouse = true;
	float GameController::lastFrame = .0f;
	float GameController::deltaTime = .0f;

	Camera GameController::mainCamera = Camera(glm::vec3(0.0f, 4.0f, 3.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	GameMode GameController::gameMode = Wandering;
	int GameController::sthSelected = 0;

	// 函数定义
	void GameController::framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void GameController::mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		static float lastX = SCR_WIDTH / 2.0f;
		static float lastY = SCR_HEIGHT / 2.0f;

		if (gameMode == Creating) {
			
		}
		else if(gameMode == Wandering) {
			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

			lastX = xpos;
			lastY = ypos;

			mainCamera.ProcessMouseMovement(xoffset, yoffset);
		}
	}

	void GameController::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		mainCamera.ProcessMouseScroll(gameMode == Wandering ? FOVY_CHANGE : HEIGHT_CHANGE, yoffset);
	}

	void GameController::processInput(GLFWwindow* window)
	{
		

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (gameMode == Creating)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(NORTH, deltaTime);
			else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(SOUTH, deltaTime);
			else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(WEST, deltaTime);
			else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(EAST, deltaTime);
		}
		else if (gameMode == Wandering)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(FORWARD, deltaTime);
			else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
			else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(LEFT, deltaTime);
			else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				mainCamera.ProcessKeyboard(RIGHT, deltaTime);
		}
	}

	void GameController::updateCursorMode(GLFWwindow* window)
	{
		
	}
}
#pragma once
#include <Camera.h>
#include <glm/glm.hpp>

namespace KooNan
{
	enum GameMode
	{
		Pause,
		Wandering,
		Creating
	};
	class GameController
	{
		// 全局状态
	public:
		static unsigned int SCR_WIDTH, SCR_HEIGHT;
		static bool firstMouse;
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
	unsigned int GameController::SCR_WIDTH = 800;
	unsigned int GameController::SCR_HEIGHT = 600;

	bool GameController::firstMouse = true;
	float GameController::lastFrame = .0f;
	float GameController::deltaTime = .0f;

	Camera GameController::mainCamera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), YAW, PITCH);

	GameMode GameController::gameMode = Pause;
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
		mainCamera.ProcessMouseScroll(yoffset);
	}

	void GameController::processInput(GLFWwindow* window)
	{
		static bool lastLAltDown = false;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			mainCamera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			mainCamera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			mainCamera.ProcessKeyboard(RIGHT, deltaTime);
	}

	void GameController::updateCursorMode(GLFWwindow* window)
	{
		
	}
}
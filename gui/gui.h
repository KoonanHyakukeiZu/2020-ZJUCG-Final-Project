#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GameController.h>

namespace KooNan
{
	enum class GUIState {Title, SightSeeing, EditScene, EditBuilding, Pause};
	int imguiDefaultFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground;

	class GUI
	{
	public:
		// init: 初始化GUI环境
		//   在GLFW初始化和窗口创建后调用
		//   在绘制GUI前调用
		static void initEnv(GLFWwindow* window) {
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();

			// Setup Platform/Renderer bindings
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init();

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();


		}

		// newFrame: 绘制前的准备
		//   每次绘制循环都需要调用
		static void newFrame() {
			// feed inputs to dear imgui, start new frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		// setWidgets: 绘制所有控件，处理输入
		//   在initEnv后调用
		static void drawWidgets() {
			ImGui::Begin("Menu");
			if (GameController::gameMode == Creating)
				if (ImGui::Button("Wandering"))
					GameController::gameMode = Wandering;
				else;
			else if (GameController::gameMode == Wandering)
				if (ImGui::Button("Creating"))
				{
					GameController::mainCamera = Camera();
					GameController::gameMode = Creating;
				}
				else;
			ImGui::End();

			// Render dear imgui into screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		// changeStateTo: 改变状态机的位置
		static void changeStateTo(GUIState nextState) {
			preState = curState;
			curState = nextState;
		}

		// revertState：退回前一状态，理论上来说只有pause会用
		static void revertState() {
			GUIState tmp = curState;
			curState = preState;
			preState = tmp;
		}

	private:
		static GUIState curState, preState;
	};

	GUIState GUI::preState = GUIState::Title;
	GUIState GUI::curState = GUIState::SightSeeing;
}
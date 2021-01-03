#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GameController.h>

namespace KooNan
{
	enum class GUIState {Title, Wandering, Creating, Pause};
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
		static void drawWidgets(int windowWidth, int windowHeight) {
			ImGui::Begin("Menu", 0, imguiDefaultFlags);
			switch (curState)
			{
			case KooNan::GUIState::Title:
				// todo：将窗口放到正中绘制菜单

				break;
			case KooNan::GUIState::Wandering:
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (!hideGui && ImGui::Button("Pause")) {
					changeStateTo(GUIState::Pause);
					GameController::gameMode = Pause;
				}
				if (!hideGui && ImGui::Button("Begin Record")) {
					// todo
				}
				if ((!hideGui && ImGui::Button("Hide UI")) || (hideGui && ImGui::Button("Show UI"))) {
					hideGui = !hideGui;
				}
				if (!hideGui && ImGui::Button("Edit Scene")) {
					changeStateTo(GUIState::Creating);
					GameController::mainCamera = Camera();
					GameController::gameMode = Creating;
				}
				break;
			case KooNan::GUIState::Creating:
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (ImGui::Button("Pause")) {
					changeStateTo(GUIState::Pause);
					GameController::gameMode = Pause;
				}
				if (ImGui::Button("Save")) {
					// todo
				}
				if (ImGui::Button("Wander")) {
					// 检查当前是否选中了建筑且位置不合法
					if (1) {
						changeStateTo(GUIState::Wandering);
						GameController::gameMode = Wandering;
					}
				}
				break;
			case KooNan::GUIState::Pause:
				// todo：改变menu位置到正中
				if (ImGui::Button("Continue")) {
					revertState();
					if (curState == GUIState::Creating) GameController::gameMode = Creating;
					else if (curState == GUIState::Wandering) GameController::gameMode = Wandering;
				}
				if (ImGui::Button("Save and Quit to Title")) {
					// todo：检查当前是否选中了建筑且位置不合法
					if (1) {
						changeStateTo(GUIState::Title);
						GameController::gameMode = Tile;
					}
				}
				break;
			default:
				break;
			}
			ImGui::End();

			if (curState == GUIState::Creating) {
				// todo：绘制选择建筑界面
				ImGui::Begin("Select Page");
				ImGui::End();

				// todo：有选中建筑时，绘制选中建筑周围的菜单
				if (1) {

				}
			}

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
		static bool hideGui;
	};

	GUIState GUI::preState = GUIState::Title;
	GUIState GUI::curState = GUIState::Wandering;
	bool GUI::hideGui = false;
}
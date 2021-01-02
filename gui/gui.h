#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
			// Set all widgets
			switch (curState)
			{
			case KooNan::GUIState::Title:

				break;
			case KooNan::GUIState::SightSeeing:
				ImGui::Begin("DefaultButtons1", NULL, imguiDefaultFlags);
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (ImGui::Button("Hello!")) {
					preState = curState;
					curState = GUIState::EditScene;
				}
				ImGui::End(); 
				break;
			case KooNan::GUIState::EditScene:
				ImGui::Begin("DefaultButtons1", NULL, imguiDefaultFlags);
				ImGui::SetWindowPos(ImVec2(600, 500));
				if (ImGui::Button("pause!")) {
					preState = curState;
					curState = GUIState::Pause;
				}
				ImGui::End();
				break;
			case KooNan::GUIState::EditBuilding:

				break;
			case KooNan::GUIState::Pause:

				break;
			default:
				break;
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

		// getCurState: 获取当前状态
		static GUIState getCurState() noexcept { return curState; }

		// 仔细想想好像没什么用，先放着吧
		static void ProcessMouseMovement(double xpos, double ypos) {};
	private:
		static GUIState curState, preState;
	};

	GUIState GUI::preState = GUIState::Title;
	GUIState GUI::curState = GUIState::SightSeeing;
}
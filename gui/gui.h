#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GameController.h>

namespace KooNan
{
	int menuFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground;
	int selectPageFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

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

		// setWidgets: 绘制所有控件
		//   在initEnv后调用
		static void drawWidgets(int windowWidth, int windowHeight) {
			static ImVec2 menuButtonSize(buttonWidth1, buttonHeight1);
			static ImVec2 shotcutButtonSize(buttonWidth2, buttonHeight2);
			ImGui::Begin("Menu", 0, menuFlags);
			switch (GameController::gameMode)
			{
			case GameMode::Title:
				ImGui::SetWindowPos(ImVec2((windowWidth - menuWidth) / 2, windowHeight * 2 / 3));
				if (ImGui::Button("Start a New Game", menuButtonSize)) {
					GameController::gameMode = Wandering;
				}
				if (ImGui::Button("Load a Game", menuButtonSize)) {

				}
				if (ImGui::Button("Exit", menuButtonSize)) {
					exit(0);
				}
				break;
			case GameMode::Wandering:
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (!hideGui && ImGui::Button("Pause", shotcutButtonSize)) {
					GameController::gameMode = Pause;
				}
				if (!hideGui && ImGui::Button("Begin Record", shotcutButtonSize)) {
					// todo
				}
				if ((!hideGui && ImGui::Button("Hide UI", shotcutButtonSize)) || (hideGui && ImGui::Button("Show UI", shotcutButtonSize))) {
					hideGui = !hideGui;
				}
				if (!hideGui && ImGui::Button("Edit Scene", shotcutButtonSize)) {
					GameController::mainCamera = GameController::oriCreatingCamera;
					GameController::gameMode = Creating;
				}
				else;
				break;
			case GameMode::Creating:
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (ImGui::Button("Pause", shotcutButtonSize)) {
					GameController::lastGameMode = GameController::gameMode;
					GameController::gameMode = Pause;
				}
				if (ImGui::Button("Save", shotcutButtonSize)) {
					// todo
				}
				if (ImGui::Button("Wander", shotcutButtonSize)) {
					// 检查当前是否选中了建筑且位置不合法
					if (1) {
						GameController::gameMode = Wandering;
					}
				}
				break;
			case GameMode::Pause:
				ImGui::SetWindowPos(ImVec2((windowWidth - menuWidth) / 2, windowHeight * 2 / 3));
				if (ImGui::Button("Continue", menuButtonSize)) {
					GameController::gameMode = GameController::lastGameMode;
				}
				if (ImGui::Button("Save and Quit to Title", menuButtonSize)) {
					// todo：检查当前是否选中了建筑且位置不合法
					if (1) {
						GameController::gameMode = Title;
					}
				}
				break;
			default:
				break;
			}
			ImGui::End();

			if (GameController::gameMode == GameMode::Creating) {
				// todo：绘制选择建筑界面
				int pageHeight = windowHeight / 4;
				pageHeight = pageHeight < 150 ? 150 : pageHeight;
				ImGui::Begin("Select Page", 0, selectPageFlags);
				ImGui::SetWindowPos(ImVec2(10, windowHeight - 10 - pageHeight));
				ImGui::SetWindowSize(ImVec2(windowWidth - 20, pageHeight));
				ImGui::End();

				// todo：有选中建筑时，绘制选中建筑周围的菜单
				if (1) {

				}
			}

			// Render dear imgui into screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		//// changeStateTo: 改变状态机的位置
		//static void changeStateTo(GameMode nextState) {
		//	preState = curState;
		//	curState = nextState;
		//}
		//// revertState：退回前一状态，理论上来说只有pause会用
		//static void revertState() {
		//	GameMode tmp = curState;
		//	curState = preState;
		//	preState = tmp;
		//}
	private:
		static bool hideGui;
		static const int buttonWidth1 = 200, buttonHeight1 = 30;
		static const int buttonWidth2 = 100, buttonHeight2 = 17;
		static const int menuWidth = 200;
	};
	bool GUI::hideGui = false;
}
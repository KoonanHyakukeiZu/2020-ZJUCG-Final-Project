#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace KooNan
{
	enum class GUIState {Pause, EditScene, EditBuilding};

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
		static void drawWidgets() {
			/*
			ImGui::Begin("Demo window");
			ImGui::Button("Hello!");
			ImGui::End();*/

			// Render dear imgui into screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

	private:
		static GUIState curState;
	};

	GUIState GUI::curState = GUIState::Pause;
}
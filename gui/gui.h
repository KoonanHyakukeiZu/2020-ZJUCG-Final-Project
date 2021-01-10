#pragma once

#include <model.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GameController.h>

namespace KooNan
{
	int menuFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground;
	int selectPageFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

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

		// Use preview.png instead.
		static void updateModelTextures(Shader& shader) {
			modelTextures.clear();
			int ntextures = Model::modelList.size();
			GLuint* textures = new GLuint[ntextures];
			GLuint* framebuffers = new GLuint[ntextures];
			glGenTextures(ntextures, textures);
			glGenFramebuffers(ntextures, framebuffers);
			modelTextures.resize(ntextures);
			frameBuffers.resize(ntextures);

			int i = 0;
			for (pair<const string, Model*> p : Model::modelList) {
				GLuint curTexture = textures[i], curFrameBuffer = framebuffers[i];
				modelTextures[i] = curTexture;
				frameBuffers[i] = curFrameBuffer;
				glBindTexture(GL_TEXTURE_2D, curTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 480, 360, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glBindFramebuffer(GL_FRAMEBUFFER, curFrameBuffer);
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, curTexture, 0);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);

				glViewport(0, 0, 480, 360);
				i++;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);
			delete[] textures, framebuffers;
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
			static ImVec2 menuButtonSize(buttonWidth1, buttonHeight1);
			static ImVec2 shotcutButtonSize(buttonWidth2, buttonHeight2);
			ImGui::Begin("Menu", 0, menuFlags);
			switch (GameController::gameMode)
			{
			case GameMode::Title:
				ImGui::SetWindowPos(ImVec2((Common::SCR_WIDTH - menuWidth) / 2, Common::SCR_HEIGHT * 2 / 3));
				if (ImGui::Button("Start a New Game", menuButtonSize)) {
					GameController::changeGameModeTo(GameMode::Wandering);
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
					GameController::changeGameModeTo(GameMode::Pause);
				}
				if (!hideGui && ImGui::Button("Begin Record", shotcutButtonSize)) {
					//changeStateTo(GUIState::Recording);
					// todo: emit record start
				}
				if ((!hideGui && ImGui::Button("Hide UI", shotcutButtonSize)) || (hideGui && ImGui::Button("Show UI", shotcutButtonSize))) {
					hideGui = !hideGui;
				}
				if (!hideGui && ImGui::Button("Edit Scene", shotcutButtonSize)) {
					GameController::changeGameModeTo(GameMode::Creating);
				}
				else;
				break;
				/*
			case KooNan::GUIState::Recording:
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (!hideGui && ImGui::Button("End Record", shotcutButtonSize)) {
					changeStateTo(GUIState::Wandering);
					// todo: emit record end
				}
				break;*/
			case GameMode::Creating:
				ImGui::SetWindowPos(ImVec2(0, 0));
				if (ImGui::Button("Pause", shotcutButtonSize)) {
					GameController::changeGameModeTo(GameMode::Pause);
				}
				if (ImGui::Button("Save", shotcutButtonSize)) {
					// todo
				}
				if (ImGui::Button("Wander", shotcutButtonSize)) {
					// 检查当前是否选中了建筑且位置不合法
					if (1) {
						GameController::changeGameModeTo(GameMode::Wandering);
					}
				}
				break;
			case GameMode::Pause:
				ImGui::SetWindowPos(ImVec2((Common::SCR_WIDTH - menuWidth) / 2, Common::SCR_HEIGHT * 2 / 3));
				if (ImGui::Button("Continue", menuButtonSize)) {
					GameController::revertGameMode();
				}
				if (ImGui::Button("Save and Quit to Title", menuButtonSize)) {
					// todo：检查当前是否选中了建筑且位置不合法
					if (1) {
						GameController::changeGameModeTo(GameMode::Title);
					}
				}
				break;
			default:
				break;
			}
			ImGui::End();

			if (GameController::gameMode == GameMode::Creating) {
				if (GameController::creatingMode == CreatingMode::Placing) {
					int pageHeight = Common::SCR_HEIGHT / 4;
					pageHeight = pageHeight < 150 ? 150 : pageHeight;
					ImVec2 selectButtonSize((pageHeight - 30) / 3 * 4, pageHeight - 30);
					ImGui::Begin("Select Page", 0, selectPageFlags);
					ImGui::SetWindowPos(ImVec2(10, Common::SCR_HEIGHT - 10 - pageHeight));
					ImGui::SetWindowSize(ImVec2(Common::SCR_WIDTH - 20, pageHeight));

					/*
					int i = 0;
					for (pair<const string, Model*> p : Model::modelList) {
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffers[i]);
						glViewport(0, 0, 480, 360);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						glGenerateMipmap(GL_TEXTURE_2D);
						p.second->Draw(NULL);
						i++;
					}
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
					glViewport(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);
					*/

					int i = 0;
					for (pair<const string, Model*> p : Model::modelList) {
						if (i) ImGui::SameLine();

						if (p.second->previewImage) {
							if (ImGui::ImageButton((void*)(p.second->previewImage->id), selectButtonSize)) {
								GameController::selectedModel = p.first;
								GameController::creatingMode = CreatingMode::Placing;
							}
						}
						else {
							if (ImGui::Button("Preview Image not Found", selectButtonSize)) {
								GameController::selectedModel = p.first;
								GameController::creatingMode = CreatingMode::Placing;
							}
						}
						i++;
					}

					ImGui::End();

				}
				else if (GameController::creatingMode == CreatingMode::Editing) {
					// todo：把菜单移动到选中建筑周围
					ImGui::Begin("Edit Menu", 0, menuFlags);
					ImGui::SetWindowPos(ImVec2(Common::SCR_WIDTH - shotcutButtonSize.x - 10, 0));
					if (ImGui::Button("Move", shotcutButtonSize)) {

					}
					if (ImGui::Button("Delete", shotcutButtonSize)) {

					}
					if (ImGui::Button("OK", shotcutButtonSize)) {
						GameController::creatingMode = CreatingMode::Placing;
					}
					ImGui::End();
				}
				else if (GameController::creatingMode == CreatingMode::Placing) {
					ImGui::Begin("Edit Menu", 0, menuFlags);
					ImGui::SetWindowPos(ImVec2(Common::SCR_WIDTH - shotcutButtonSize.x - 10, 0));
					if (ImGui::Button("Cancel", shotcutButtonSize)) {
						GameController::creatingMode = CreatingMode::Placing;
						GameController::selectedModel = "";
					}
					ImGui::End();
				}
			}

			// Render dear imgui into screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	private:
		static bool hideGui;
		static vector<GLuint> modelTextures, frameBuffers;
		static const int buttonWidth1 = 200, buttonHeight1 = 30;
		static const int buttonWidth2 = 100, buttonHeight2 = 17;
		static const int menuWidth = 200;
	};
	bool GUI::hideGui = false;
	vector<GLuint> GUI::modelTextures, GUI::frameBuffers;
}
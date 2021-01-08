#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <scene.h>
#include <light.h>
#include <vector>
namespace KooNan
{
    class Render
    {
	private:
		Scene& main_scene;
		Light& main_light;
		Water_Frame_Buffer& waterfb;
		PickingTexture& mouse_picking;
	public:
		Render(Scene& main_scene, Light& main_light, Water_Frame_Buffer& waterfb, PickingTexture& mouse_picking):
			main_scene(main_scene), main_light(main_light),waterfb(waterfb), mouse_picking(mouse_picking)
		{
			main_scene.TerrainShader.use();
			main_light.SetLight(main_scene.TerrainShader);

			main_scene.WaterShader.use();
			main_light.SetLight(main_scene.WaterShader);
		}
		void InitObjectLighting(Shader& shader)
		{
			main_light.SetLight(shader);
		}
		void DrawReflection(Shader& modelShader)
		{
			glm::vec4 clipping_plane = glm::vec4(0.0, 1.0, 0.0, -main_scene.getWaterHeight());
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

			DrawObjects(modelShader, clipping_plane, false);

			// we now draw as many light bulbs as we have point lights.
			main_light.Draw(GameController::mainCamera, clipping_plane);
			//render the main scene
			main_scene.Draw(GameController::deltaTime, GameController::mainCamera, clipping_plane, false);

			//Restore the main camera
			GameController::mainCamera.Position.y += distance;
			GameController::mainCamera.Pitch = -GameController::mainCamera.Pitch;
			GameController::mainCamera.GetViewMatrix();

			waterfb.unbindCurrentFrameBuffer();
		}
		void DrawRefraction(Shader& modelShader)
		{
			glm::vec4 clipping_plane = glm::vec4(0.0, -1.0, 0.0, main_scene.getWaterHeight());
			glEnable(GL_CLIP_DISTANCE0);
			waterfb.bindRefractionFrameBuffer();
			glEnable(GL_DEPTH_TEST);
			// render
			// ------
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			DrawObjects(modelShader, clipping_plane, false);

			// we now draw as many light bulbs as we have point lights.
			main_light.Draw(GameController::mainCamera, clipping_plane);
			//render the main scene
			main_scene.Draw(GameController::deltaTime, GameController::mainCamera, clipping_plane, false);

			waterfb.unbindCurrentFrameBuffer();
		}
		void DrawAll(Shader& pickingShader,Shader& modelShader)
		{
			glm::vec4 clipping_plane = glm::vec4(0.0, -1.0, 0.0, 99999.0f);
			mouse_picking.bindFrameBuffer();
			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			PickObjects(pickingShader);
			mouse_picking.unbindFrameBuffer();


			// render
			// ------
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			DrawObjects(modelShader, clipping_plane, true);
			main_light.Draw(GameController::mainCamera, clipping_plane);


			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			main_scene.setReflectText(waterfb.getReflectionTexture());
			main_scene.setRefractText(waterfb.getRefractionTexture());
			main_scene.setDepthMap(waterfb.getRefractionDepthTexture());
			main_scene.Draw(GameController::deltaTime, GameController::mainCamera, clipping_plane, true);


			glDisable(GL_BLEND);
		}
		private:
			void DrawObjects(Shader& modelShader, glm::vec4 clippling_plane, bool IsAfterPicking)
			{

				for (int i = 0; i < GameObject::gameObjList.size(); i++)
				{
					bool intersected = false;
					
					if (IsAfterPicking)
					{
						float hitObjID = mouse_picking.ReadPixel(GameController::cursorX, 
							Common::SCR_HEIGHT - GameController::cursorY - 22).ObjID;//deviation of y under resolution 1920*1080 maybe 22
						if ((unsigned int)hitObjID == i + 1)
						{
							intersected = true;
						}
					}
					GameObject::gameObjList[i]->Draw(modelShader, GameController::mainCamera.Position,
						Common::GetPerspectiveMat(GameController::mainCamera), GameController::mainCamera.GetViewMatrix(),
						clippling_plane,
						intersected);
				}
			}
			void PickObjects(Shader& modelShader)
			{
				unsigned int object_counter = 0;
				for (int i = 0; i < GameObject::gameObjList.size(); i++)
				{
					if (GameObject::gameObjList[i]->IsPickable)
						GameObject::gameObjList[i]->Pick(modelShader, ++object_counter, 0,
							Common::GetPerspectiveMat(GameController::mainCamera), GameController::mainCamera.GetViewMatrix());

				}

			}
			
    };
}
#endif

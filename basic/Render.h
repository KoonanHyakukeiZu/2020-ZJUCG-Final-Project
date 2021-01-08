#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <scene.h>
#include <light.h>

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

		}
		void DrawReflection()
		{
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

			//========================================================================
			//Draw objects
			//========================================================================

			// we now draw as many light bulbs as we have point lights.
			main_light.Draw(GameController::mainCamera, glm::vec4(0.0, 1.0, 0.0, -main_scene.getWaterHeight()));
			//render the main scene
			main_scene.Draw(GameController::deltaTime, GameController::mainCamera, glm::vec4(0.0, 1.0, 0.0, -main_scene.getWaterHeight()), false);

			//Restore the main camera
			GameController::mainCamera.Position.y += distance;
			GameController::mainCamera.Pitch = -GameController::mainCamera.Pitch;
			GameController::mainCamera.GetViewMatrix();

			waterfb.unbindCurrentFrameBuffer();
		}
		void DrawRefraction()
		{
			glEnable(GL_CLIP_DISTANCE0);
			waterfb.bindRefractionFrameBuffer();
			glEnable(GL_DEPTH_TEST);
			// render
			// ------
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//========================================================================
			//Draw objects
			//========================================================================

			// we now draw as many light bulbs as we have point lights.
			main_light.Draw(GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, main_scene.getWaterHeight()));
			//render the main scene
			main_scene.Draw(GameController::deltaTime, GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, main_scene.getWaterHeight()), false);

			waterfb.unbindCurrentFrameBuffer();
		}
		void DrawAll(Shader& pickingShader)
		{
			mouse_picking.bindFrameBuffer();
			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//========================================================================
			//Draw picking objects
			//========================================================================
			mouse_picking.unbindFrameBuffer();


			// render
			// ------
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//========================================================================
			//Draw objects
			//========================================================================
			main_light.Draw(GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, 99999.0f));


			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			main_scene.setReflectText(waterfb.getReflectionTexture());
			main_scene.setRefractText(waterfb.getRefractionTexture());
			main_scene.setDepthMap(waterfb.getRefractionDepthTexture());
			main_scene.Draw(GameController::deltaTime, GameController::mainCamera, glm::vec4(0.0, -1.0, 0.0, 99999.0f), true);


			glDisable(GL_BLEND);
		}
		private:
			void DrawObjects()
			{

			}
			
    };
}
#endif

#ifndef MOUSEPICKER_H
#define MOUSEPICKER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common.h>

#include <Camera.h>

namespace KooNan
{
	class MousePicker {
	private:
		glm::vec3 currentRay;
		Camera& cam;
		glm::mat4 projection;
		glm::mat4 view;
	public:
		MousePicker(Camera& cam) :cam(cam)
		{
			view = cam.GetViewMatrix();
			projection = Common::GetPerspectiveMat(cam);
		}
		glm::vec3 getCurrentRay()
		{
			return currentRay;
		}
		void update(double mx, double my)
		{
			view = cam.GetViewMatrix();
			currentRay = calculateMouseRay(mx, my);
		}
		bool ifIntersected(glm::vec3 model_position, float collision_radius)
		{
			glm::vec3 view_to_center = cam.Position - model_position;
			float b = glm::dot(currentRay, view_to_center);
			float c = glm::dot(view_to_center, view_to_center) - collision_radius * collision_radius;
			float crition = b * b - c;
			if (crition >= 0)
				return true;
			else
				return false;
		}
	private:
		glm::vec3 calculateMouseRay(int x, int y)
		{
			glm::vec2 normalizedCoord = getNDC(x, y);
			glm::vec4 clipCoord = glm::vec4(normalizedCoord.x, normalizedCoord.y, -1.0f, 0.0f);
			glm::vec4 eyeCoord = toEyeCoord(clipCoord);
			glm::vec3 worldCoord = toWorldCoord(eyeCoord);
			return worldCoord;
		}
		glm::vec2 getNDC(int mx, int my)
		{
			float x = (float)(2.0f*mx / Common::SCR_WIDTH) - 1;
			float y = (float)(2.0f*my / Common::SCR_HEIGHT) - 1;
			return glm::vec2(x, -y);
		}
		glm::vec4 toEyeCoord(glm::vec4 clipCoord)
		{
			glm::mat4 projectionInversed = glm::inverse(projection);
			glm::vec4 eyeCoord = projectionInversed * clipCoord;
			return glm::vec4(eyeCoord.x, eyeCoord.y, -1.0f, 0.0f);
		}
		glm::vec3 toWorldCoord(glm::vec4 eyeCoord)
		{
			glm::mat4 viewInversed = glm::inverse(view);
			glm::vec4 worldCoord = viewInversed * eyeCoord;
			return glm::normalize(glm::vec3(worldCoord.x, worldCoord.y, worldCoord.z));
		}
	};
}

#endif
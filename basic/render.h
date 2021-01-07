#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace KooNan
{
	class Render 
	{
	public:
		static unsigned int SCR_WIDTH, SCR_HEIGHT;
		static float perspective_clipping_near;
		static float perspective_clipping_far;
		static glm::mat4 GetPerspectiveMat(Camera& cam)
		{
			return glm::perspective(glm::radians(cam.Zoom),
				(float)Render::SCR_WIDTH / (float)Render::SCR_HEIGHT, 
				Render::perspective_clipping_near, Render::perspective_clipping_far);
		}

	};

	unsigned int Render::SCR_WIDTH = 1920;
	unsigned int Render::SCR_HEIGHT = 1080;
	float Render::perspective_clipping_near = 0.1f;
	float Render::perspective_clipping_far = 1000.0f;
}

#endif
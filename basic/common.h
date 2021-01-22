#ifndef COMMON_H
#define COMMON_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace KooNan
{
	class Common 
	{
	public:
		static unsigned int SCR_WIDTH, SCR_HEIGHT;
		static float perspective_clipping_near;
		static float perspective_clipping_far;
		static const string saveFileName;
		static glm::mat4 GetPerspectiveMat(Camera& cam)
		{
			return glm::perspective(glm::radians(cam.Zoom),
				(float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT,
				Common::perspective_clipping_near, Common::perspective_clipping_far);
		}

	};

	unsigned int Common::SCR_WIDTH = 1920;
	unsigned int Common::SCR_HEIGHT = 1080;
	float Common::perspective_clipping_near = 0.1f;
	float Common::perspective_clipping_far = 1000.0f;
	const string Common::saveFileName = "Save.json";
}

#endif
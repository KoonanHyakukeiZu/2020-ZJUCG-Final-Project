#ifndef SKYBOX_H
#define SKYBOX_H

#ifndef STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#endif 

#include <iostream>
#include <vector>
#include <string>

// GLM Math Library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD: A library that wraps OpenGL functions to make things easier
//       Note that GLAD MUST be included before GLFW
#include "glad/glad.h"

#include "Shader.h"

namespace KooNan
{
	class Skybox
	{
	public:
		Skybox(std::vector<std::string> paths);

		void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

		unsigned int getCubeMap() { return texture; }
	private:
		unsigned int VAO, VBO, texture;

		unsigned int GenCubeMap(std::vector<std::string> facePaths);
	};

	static float skyboxVertices[] = {
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	Skybox::Skybox(std::vector<std::string> paths)
	{
		texture = GenCubeMap(paths);

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	void Skybox::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{
		glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setInt("skybox", 0);
		glDepthMask(GL_FALSE);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
	}

	unsigned int Skybox::GenCubeMap(std::vector<std::string> facePaths)
	{
		if (facePaths.size() < 6) {
			std::cout << "Cube map texture not enough! " << std::endl;
		}

		unsigned int tid;
		glGenTextures(1, &tid);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tid);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (unsigned int i = 0; i < facePaths.size(); ++i)
		{
			unsigned char *image = stbi_load(facePaths[i].c_str(), &width, &height, &nrChannels, 0);
			if (image)
			{

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0,
					GL_RGB, GL_UNSIGNED_BYTE, image);
				stbi_image_free(image);
			}
			else
			{
				std::cout << "Cube map texture failed to load at path: " << facePaths[i] << std::endl;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		return tid;
	}
}
#endif // !SKYBOX_H

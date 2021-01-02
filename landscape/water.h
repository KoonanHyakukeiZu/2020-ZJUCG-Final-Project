#ifndef WATER_H
#define WATER_H

#include <glad/glad.h>

namespace KooNan
{
	class Water
	{
	private:
		float size;
		int index_x;
		int index_z;
		float height;
		unsigned int VBO;

	public:
		unsigned int VAO;
		Water(int grid_index_x, int grid_index_z, float water_height = -0.5f, float chunk_size = 32.0f)
		{
			this->size = chunk_size;
			this->index_x = grid_index_x;
			this->index_z = grid_index_z;
			this->height = water_height;
			setUp(grid_index_x, grid_index_z, chunk_size, water_height);
		}
		float getHeight()
		{
			return this->height;
		}
		void Draw(Shader &shader)
		{
			shader.use();
			glBindVertexArray(VAO);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	private:
		void setUp(int grid_index_x, int grid_index_z, float chunk_size, float water_height)
		{
			//boundaries of the water chunk
			float Left = -chunk_size / 2.0f + grid_index_x * chunk_size;
			float Right = -chunk_size / 2.0f + (grid_index_x + 1) * chunk_size;
			float Up = -chunk_size / 2.0f + grid_index_z * chunk_size;
			float Down = -chunk_size / 2.0f + (grid_index_z + 1) * chunk_size;
			float vertices[] = {
				Left,water_height,Up,//UpLeft
				Left,water_height,Down,//DownLeft
				Right,water_height,Up,//UpRight
				Right,water_height,Up,//UpRight
				Left,water_height,Down,//DownLeft
				Right,water_height,Down//DownRight
			};
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glBindVertexArray(0);
		}


	};
}


#endif
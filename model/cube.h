#ifndef CUBE_H
#define CUBE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GameController.h>
#include <Render.h>
#include <Camera.h>
#include <mesh.h>
#include <vector>


namespace KooNan
{
	class Cube
	{
	private:
		Mesh CubeMesh;
		Shader& CubeShader;
		Mesh InitCube(vector<Texture> inp_textures)
		{
			float vertices[] = {
				// positions          // normals           // texture coords
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
				 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
				 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
				 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
				 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
				 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
				 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
			};
			vector<Vertex_Simple>       cube_vertices;
			vector<unsigned int>		cube_indices;
			vector<Texture>				cube_textures = inp_textures;
			Vertex_Simple* tmp_vertex = new Vertex_Simple;
			for (unsigned int i = 0; 8 * i < (sizeof(vertices) / sizeof(float)); i++)
			{
				tmp_vertex->Position = glm::vec3(vertices[8 * i + 0], vertices[8 * i + 1], vertices[8 * i + 2]);
				tmp_vertex->Normal = glm::vec3(vertices[8 * i + 3], vertices[8 * i + 4], vertices[8 * i + 5]);
				tmp_vertex->TexCoords = glm::vec3(vertices[8 * i + 6], vertices[8 * i + 7], vertices[8 * i + 8]);
				cube_vertices.push_back(*tmp_vertex);
				for (unsigned int j = 0; j < 8; j++)
				{
					cube_indices.push_back(8 * i + j);
				}

			}
			delete tmp_vertex;
			return Mesh(cube_vertices, cube_indices, cube_textures);
		}
	public:
		unsigned int VAO, VBO;
		Cube(vector<Texture> textures, Shader& CubeShader) :CubeMesh(InitCube(textures)), CubeShader(CubeShader)
		{ }
		~Cube()
		{
			CubeMesh.cleanUp();
		}
		void Draw(Camera& cam, glm::vec4 clippling_plane, glm::mat4 model, glm::mat4 projection, bool if_hit = false)
		{
			CubeShader.use();
			if (if_hit)
				CubeShader.setVec3("selected_color", glm::vec3(0.5f,0.5f,0.5f));
			else
				CubeShader.setVec3("selected_color", glm::vec3(0.0f, 0.0f, 0.0f));
			CubeShader.setMat4("projection", projection);
			CubeShader.setMat4("view", cam.GetViewMatrix());
			CubeShader.setVec4("plane", clippling_plane);
			CubeShader.setVec3("viewPos", cam.Position);
			CubeShader.setMat4("model", model);
			CubeMesh.Draw(&CubeShader);
		}
		void Pick(Shader& pickingShader, Camera& cam, glm::mat4 model, glm::mat4 projection, unsigned int objIndex, unsigned int drawIndex)
		{
			pickingShader.use();
			pickingShader.setMat4("projection", projection);
			pickingShader.setMat4("view", cam.GetViewMatrix());
			pickingShader.setMat4("model", model);
			pickingShader.setUint("drawIndex", drawIndex);
			pickingShader.setUint("objIndex", objIndex);
			CubeMesh.Draw(&pickingShader);
		}
		
	};
}
#endif // !CUBE_H

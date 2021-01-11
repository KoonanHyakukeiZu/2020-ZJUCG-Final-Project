
#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

#include <mesh.h>
#include <terrain_random.h>
#include <vector>
#include <string>

namespace KooNan
{
	class Terrain {
	private:
		float size;
		int vertex_count;
		int index_x;
		int index_z;
		float world_x;
		float world_z;
		bool flatten;
		std::vector<float> land_heights;
		Mesh terrain_mesh;	
	public:
		Terrain(int grid_index_x, int grid_index_z, vector<Texture> texture, float chunk_size = 32.0f, int vertex_count = 32, float if_flatten = false) :
			size(chunk_size), vertex_count(vertex_count), index_x(grid_index_x), index_z(grid_index_z), 
			world_x(index_x * size), world_z(index_z * size), flatten(if_flatten), terrain_mesh(generateTerrain(texture, if_flatten))
		{}
		Terrain(int grid_index_x, int grid_index_z, vector<Texture> texture, string heightmap_path, float chunk_size = 32.0f, int vertex_count = 32):
			size(chunk_size), index_x(grid_index_x),index_z(grid_index_z), world_x(index_x * size), world_z(index_z * size),flatten(false), terrain_mesh(LoadTerrain(texture,heightmap_path))
		{}
		void Draw(Shader &shader)
		{
			this->terrain_mesh.Draw(&shader);
		}
		float GetTerrainHeight(float x, float z)
		{
			float relativeX = x - world_x + size / 2;
			float relativeZ = z - world_z + size / 2;
			if (!(relativeX >= 0 && relativeX <= size))
				return 0;
			if (!(relativeZ >= 0 && relativeZ <= size))
				return 0;
			float gridSqaureSize = size / (vertex_count - 1);
			int gridX = (int)(relativeX / gridSqaureSize);
			int gridZ = (int)(relativeZ / gridSqaureSize);
			float xCoord = (relativeX - gridX * gridSqaureSize) / gridSqaureSize;
			float zCoord = (relativeZ - gridZ * gridSqaureSize) / gridSqaureSize;
			if (xCoord <= (1 - zCoord))
			{
				glm::vec3 p1 = glm::vec3(0, land_heights[gridX + gridZ * vertex_count], 0);
				glm::vec3 p2 = glm::vec3(1, land_heights[(gridX + 1) + gridZ * vertex_count], 0);
				glm::vec3 p3 = glm::vec3(0, land_heights[gridX + (gridZ + 1)*vertex_count], 1);
				return barryCentric(p1, p2, p3, glm::vec2(xCoord, zCoord));
			}
			else
			{
				glm::vec3 p1 = glm::vec3(1, land_heights[(gridX + 1) + (gridZ + 1) * vertex_count], 1);
				glm::vec3 p2 = glm::vec3(1, land_heights[(gridX + 1) + gridZ * vertex_count], 0);
				glm::vec3 p3 = glm::vec3(0, land_heights[gridX + (gridZ + 1)*vertex_count], 1);
				return barryCentric(p1, p2, p3, glm::vec2(xCoord, zCoord));
			}

		}
	private:
		Mesh generateTerrain(vector<Texture> texture, bool flatten = false)
		{
			this->flatten = flatten;
			this->land_heights.resize(vertex_count*vertex_count);
			HeightsGenerator hei_gen;
			int count = vertex_count * vertex_count;
			vector<Vertex_Simple>       vertices;
			vector<unsigned int>		indices;
			vector<Texture>				textures;

			Vertex_Simple* tmp_vertex = new Vertex_Simple;
			if (!flatten)
			{
				for (int i = 0; i < vertex_count; i++)
				{
					for (int j = 0; j < vertex_count; j++)
					{
						float x = world_x + ((float)j - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size;
						float z = world_z + ((float)i - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size;
						float y = hei_gen.heightsGeneration(j + index_x * (vertex_count - 1), i + index_z * (vertex_count - 1));
						(this->land_heights)[j + i * vertex_count] = y;
						tmp_vertex->Position = glm::vec3(x, y, z);
						tmp_vertex->Normal = calculateNormal(j, i, hei_gen);
						tmp_vertex->TexCoords = glm::vec2((float)j / ((float)vertex_count - 1), (float)i / ((float)vertex_count - 1));
						vertices.push_back(*tmp_vertex);
					}
				}
			}
			else
			{
				for (int i = 0; i < vertex_count; i++)
					for (int j = 0; j < vertex_count; j++)
					{
						tmp_vertex->Position = glm::vec3(((float)j - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size, 0, ((float)i - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size);
						tmp_vertex->Normal = glm::vec3(0.0f, 1.0f, 0.0f);
						tmp_vertex->TexCoords = glm::vec2((float)j / ((float)vertex_count - 1), (float)i / ((float)vertex_count - 1));
						vertices.push_back(*tmp_vertex);
					}
			}
			indices = generateIndex();
			delete tmp_vertex;
			return Mesh(vertices, indices, texture);
		}
		Mesh LoadTerrain(vector<Texture> texture, std::string heightmap_path)
		{
			int width, height, nrComponents;
			unsigned char *data = stbi_load(heightmap_path.c_str(), &width, &height, &nrComponents, 0);
			assert(width == height);//width of heightmap must be equal to height
			this->vertex_count = width;
			this->land_heights.resize(vertex_count*vertex_count);
			vector<Vertex_Simple>       vertices;
			vector<unsigned int>		indices;
	
			Vertex_Simple* tmp_vertex = new Vertex_Simple;
			for (int i = 0; i < vertex_count; i++)
			{
				for (int j = 0; j < vertex_count; j++)
				{
					float x = world_x + ((float)j - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size;
					float z = world_z + ((float)i - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size;
					float y = GetHeightFromMap(j, i, data, nrComponents, vertex_count);
					(this->land_heights)[j + i * vertex_count] = y;
					tmp_vertex->Position = glm::vec3(x, y, z);
					tmp_vertex->Normal = calculateNormalFromMap(j, i, data, nrComponents, vertex_count);
					tmp_vertex->TexCoords = glm::vec2((float)j / ((float)vertex_count - 1), (float)i / ((float)vertex_count - 1));
					vertices.push_back(*tmp_vertex);
				}
			}
			stbi_image_free(data);
			indices = generateIndex();
			delete tmp_vertex;
			return Mesh(vertices, indices, texture);
		}
		vector<unsigned int> generateIndex()
		{
			vector<unsigned int> indices;
			for (int gz = 0; gz < vertex_count - 1; gz++)
				for (int gx = 0; gx < vertex_count - 1; gx++)
				{
					int topLeft = ((gz)*vertex_count) + gx;
					int topRight = topLeft + 1;
					int bottomLeft = ((gz + 1)*vertex_count) + gx;
					int bottomRight = bottomLeft + 1;
					indices.push_back(topLeft);
					indices.push_back(bottomLeft);
					indices.push_back(topRight);
					indices.push_back(topRight);
					indices.push_back(bottomLeft);
					indices.push_back(bottomRight);
				}
			assert(indices.size() == 6 * (vertex_count - 1)*(vertex_count - 1));
			return indices;
		}
		float GetHeightFromMap(int x, int z, const unsigned char *data, int nrComponents, int map_size)
		{
			float amplitude = 20.0f;
			int rgb;
			if (x >= 0 && x < map_size && z >= 0 && z < map_size)
			{
				//std::cout << (unsigned int)data[(x + z * map_size)*nrComponents + 1] << endl;
				rgb = data[(x + z * map_size)*nrComponents + 0] * data[(x + z * map_size)*nrComponents + 1] * data[(x + z * map_size)*nrComponents + 2];
			}
			else
			{
				return 0;
			}

			float height = (rgb / 16777216.0f) - 0.125f;
			//std::cout << height << endl;
			return  height * amplitude;
		}

		glm::vec3 calculateNormal(int x, int z, HeightsGenerator generator)
		{
			float heightL = generator.heightsGeneration(x - 1, z);
			float heightR = generator.heightsGeneration(x + 1, z);
			float heightD = generator.heightsGeneration(x, z - 1);
			float heightU = generator.heightsGeneration(x, z + 1);
			glm::vec3 Norm = glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));
			return Norm;
		}

		glm::vec3 calculateNormalFromMap(int x, int z, const unsigned char *data, int nrComponents, int map_size)
		{
			float heightL = GetHeightFromMap(x - 1, z, data, nrComponents, vertex_count);
			float heightR = GetHeightFromMap(x + 1, z, data, nrComponents, vertex_count);
			float heightD = GetHeightFromMap(x, z - 1, data, nrComponents, vertex_count);
			float heightU = GetHeightFromMap(x, z + 1, data, nrComponents, vertex_count);
			glm::vec3 Norm = glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightD - heightU));
			return Norm;
		}
		float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {
			float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
			float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
			float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
			float l3 = 1.0f - l1 - l2;
			return l1 * p1.y + l2 * p2.y + l3 * p3.y;
		}




	};
}


#endif
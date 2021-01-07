
#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <mesh.h>
#include <terrain_random.h>
#include <vector>

class Terrain{
    private:
    const float size = 20;
    const int vertex_count = 32;
    float world_x;
    float world_z;
	bool flatten;
    Mesh terrain_mesh;
    Mesh generateTerrain(vector<Texture> inp_textures, bool flatten = true)
    {
		this->flatten = flatten;
		HeightsGenerator hei_gen;
        int count = vertex_count * vertex_count;
        vector<Vertex_Simple>       vertices;
        vector<unsigned int>		indices;
        vector<Texture>				textures = inp_textures;
		Vertex_Simple* tmp_vertex = new Vertex_Simple;
		if (!flatten)
		{
			for (int i = 0; i < vertex_count; i++)
				for (int j = 0; j < vertex_count; j++)
				{
					tmp_vertex->Position = glm::vec3(((float)j - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size, hei_gen.heightsGeneration(j, i), ((float)i - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size);
					tmp_vertex->Normal = calculateNormal(j, i, hei_gen);
					tmp_vertex->TexCoords = glm::vec2((float)j / ((float)vertex_count - 1), (float)i / ((float)vertex_count - 1));
					vertices.push_back(*tmp_vertex);
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
        for(int gz=0;gz<vertex_count-1;gz++)
            for(int gx=0;gx<vertex_count-1;gx++)
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
		delete tmp_vertex;
        return Mesh(vertices, indices, textures);
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
    public:
    Terrain(int grid_index_x, int grid_index_z, vector<Texture> textures):terrain_mesh(generateTerrain(textures)){
        world_x = grid_index_x * size;
        world_z = grid_index_z * size;     
    }
	void Draw(Shader &shader)
	{
		this->terrain_mesh.Draw(&shader);
	}


    
};



#endif
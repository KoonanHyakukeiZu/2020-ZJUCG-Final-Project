#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <GameController.h>
#include <Shader.h>
#include <Camera.h>
#include <terrain.h>
#include <water.h>
#include <skybox.h>
#include <Texture.h>


namespace KooNan
{
	class Scene {
	private:
		float chunk_size;
	public:
		vector<Terrain> all_terrain_chunks;
		vector<Water> all_water_chunks;
		Skybox skybox;
		vector<string> groundPaths;
		int width;
		int height;
		float water_height;
		Shader& TerrainShader;
		Shader& WaterShader;
		Shader& SkyShader;
		float waterMoveFactor;
		unsigned int reflect_text, refract_text, dudvMap, normalMap, depthMap;
	public:
		/*
		float chunk_size: define size of each chunk
		int scene_width: should be integer > 0, define the chunk grid width
		int scene_height: should be integer > 0, define the chunk grid height
		Shader& TerrainShader: Pass the terrain shader to it
		Shader& WaterShader: Pass the water shader to it
		Shader& SkyShader: Pass the skybox shader to it
		string ground_textures: Pass a string of ground texture
		vector<string> skyboxPaths: Pass a string vector contains skybox textures
		*/
		Scene(float chunk_size, int scene_width, int scene_height, float water_height, Shader& TerrainShader, Shader& WaterShader, Shader& SkyShader, vector<string> groundPaths, vector<string> skyboxPaths) :
			chunk_size(chunk_size), skybox(skyboxPaths), groundPaths(groundPaths), width(scene_width), height(scene_height),
			water_height(water_height), TerrainShader(TerrainShader), WaterShader(WaterShader), SkyShader(SkyShader)
		{
			waterMoveFactor = 0.0f;
			InitScene(groundPaths);
		}
		float getWaterHeight()
		{
			return this->water_height;
		}
		void setReflectText(unsigned int textID)
		{
			reflect_text = textID;
		}
		void setRefractText(unsigned int textID)
		{
			refract_text = textID;
		}
		void setDudvMap(unsigned int textID)
		{
			dudvMap = textID;
		}
		void setNormalMap(unsigned int textID)
		{
			normalMap = textID;
		}
		void setDepthMap(unsigned int textID)
		{
			depthMap = textID;
		}
		void Draw(float deltaTime, Camera& cam, glm::vec4 clippling_plane, bool draw_water)
		{
			glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)GameController::SCR_WIDTH / (float)GameController::SCR_HEIGHT, 0.1f, 1000.0f);
			glm::mat4 view = cam.GetViewMatrix();
			glm::vec3 viewPos = cam.Position;
			TerrainShader.use();
			TerrainShader.setMat4("projection", projection);
			TerrainShader.setMat4("view", view);
			TerrainShader.setVec4("plane", clippling_plane);
			TerrainShader.setVec3("viewPos", viewPos);
			for (int i = 0; i < all_terrain_chunks.size(); i++)
			{
				all_terrain_chunks[i].Draw(TerrainShader);
			}
			SkyShader.use();

			skybox.Draw(SkyShader, glm::scale(glm::mat4(1.0f), glm::vec3(500.0f)), view, projection);
			if (draw_water)
			{
				waterMoveFactor += deltaTime * 0.1f;
				waterMoveFactor = waterMoveFactor - (int)waterMoveFactor;
				WaterShader.use();

				WaterShader.setMat4("projection", projection);
				WaterShader.setMat4("view", view);
				WaterShader.setVec3("viewPos", viewPos);
				WaterShader.setInt("reflection", 0);
				WaterShader.setInt("refraction", 1);
				WaterShader.setInt("dudvMap", 2);
				WaterShader.setInt("normalMap", 3);
				WaterShader.setInt("depthMap", 4);
				WaterShader.setFloat("chunk_size", chunk_size);
				WaterShader.setFloat("moveOffset", waterMoveFactor);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, reflect_text);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, refract_text);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, dudvMap);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, normalMap);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, depthMap);
				for (int j = 0; j < all_water_chunks.size(); j++)
				{
					all_water_chunks[j].Draw(WaterShader);
				}
			}
		}
		float getTerrainHeight(float x, float z)
		{
			float relativeX = x + chunk_size / 2;
			float relativeZ = z + chunk_size / 2;
			int gridX = (int)(relativeX / chunk_size);
			int gridZ = (int)(relativeZ / chunk_size);
			return all_terrain_chunks[gridX*width + gridZ].GetTerrainHeight(x, z);
		}
	private:
		void InitScene(vector<string> ground_path)
		{
			bool use_heightMap = false;
			string heightMapPath;
			if (groundPaths.size() == 8)
			{
				use_heightMap = true;
				heightMapPath = *ground_path.begin();
				ground_path.erase(ground_path.begin());
				assert(ground_path.size() == 7);
			}
			vector<string> types;
			for (int k = 0; k < ground_path.size(); k++)
			{
				types.push_back("texture_diffuse");
			}
			TextureManager texman(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			vector<Texture> textures = texman.LoadTexture(ground_path, types);
			vector<Texture> ground_textures;
			for (int l = 0; l < 5; l++)
			{
				ground_textures.push_back(textures[l]);
			}
			for (int i = 0; i < width; i++)
				for (int j = 0; j < height; j++)
				{
					if (!use_heightMap)
					{
						Terrain ground(i, j, ground_textures, chunk_size);
						all_terrain_chunks.push_back(ground);
						Water water_surface(i, j, water_height, chunk_size);
						all_water_chunks.push_back(water_surface);
					}
					else
					{
						Terrain ground(i, j, ground_textures, heightMapPath, chunk_size);
						all_terrain_chunks.push_back(ground);
						Water water_surface(i, j, water_height, chunk_size);
						all_water_chunks.push_back(water_surface);
					}
				}
			WaterShader.use();
			WaterShader.setVec3("material.diffuse", glm::vec3(0.2, 0.2, 0.2));
			WaterShader.setVec3("material.specular", glm::vec3(1.0, 1.0, 1.0));
			WaterShader.setFloat("material.shininess", 128.0f);
			setDudvMap(textures[textures.size() - 2].id);
			setNormalMap(textures[textures.size() - 1].id);

		}


	};
}
#endif
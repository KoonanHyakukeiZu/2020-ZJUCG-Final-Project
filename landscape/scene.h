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



namespace KooNan
{
	class Scene {
	private:
		float chunk_size;
		vector<Terrain> all_terrain_chunks;
		vector<Water> all_water_chunks;
		Skybox skybox;
		vector<Texture> ground_textures;
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
		vector<Texture> ground_textures: Pass a texture vector contains terrain textures
		vector<string> skyboxPaths: Pass a string vector contains skybox textures
		*/
		Scene(float chunk_size, int scene_width, int scene_height, float water_height, Shader& TerrainShader, Shader& WaterShader, Shader& SkyShader, vector<Texture> ground_textures, vector<string> skyboxPaths) :
			chunk_size(chunk_size), skybox(skyboxPaths), ground_textures(ground_textures), width(scene_width), height(scene_height),
			water_height(water_height), TerrainShader(TerrainShader), WaterShader(WaterShader), SkyShader(SkyShader)
		{
			waterMoveFactor = 0.0f;
			InitScene();
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

			skybox.Draw(SkyShader, glm::scale(glm::mat4(1.0f), glm::vec3(100.0f)), view, projection);
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
	private:
		void InitScene()
		{
			for (int i = 0; i < width; i++)
				for (int j = 0; j < height; j++)
				{
					Terrain ground(i, j, ground_textures, chunk_size);
					all_terrain_chunks.push_back(ground);
					Water water_surface(i, j, water_height, chunk_size);
					all_water_chunks.push_back(water_surface);
				}
			WaterShader.use();
			WaterShader.setVec3("material.diffuse", glm::vec3(0.2, 0.2, 0.2));
			WaterShader.setVec3("material.specular", glm::vec3(1.0, 1.0, 1.0));
			WaterShader.setFloat("material.shininess", 128.0f);

		}


	};
}
#endif
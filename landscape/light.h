#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <Shader.h>
#include <Camera.h>
#include <cube.h>
#include <common.h>

namespace KooNan
{
	struct PointLight {
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};
	struct DirLight {
		glm::vec3 direction;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};
	class Light
	{
	private:
		DirLight parallel_light;
		std::vector<PointLight> point_lights;
		Shader& LightboxShader;
	public:
		Light(DirLight parallel_light, Shader& lightbox):parallel_light(parallel_light), LightboxShader(lightbox){}
		void AddPointLight(PointLight light)
		{
			point_lights.push_back(light);
		}
		void Draw(Camera& cam, glm::vec4 clipping_plane)
		{
			std::vector<Texture> textures;//Load a null texture
			Cube lightcube(textures, LightboxShader);
			
			for (int i = 0; i < point_lights.size(); i++)
			{
				glm::mat4 model(1.0f);
				model = glm::translate(model, point_lights[i].position);
				model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
				lightcube.Draw(cam, clipping_plane, model ,false);
			}
		};
		void SetLight(Shader& SomeEntities)
		{
			SomeEntities.use();
			SomeEntities.setVec3("dirLight.direction", parallel_light.direction);
			SomeEntities.setVec3("dirLight.ambient", parallel_light.ambient);
			SomeEntities.setVec3("dirLight.diffuse", parallel_light.diffuse);
			SomeEntities.setVec3("dirLight.specular", parallel_light.specular);

			std::string s_lights = "pointLights[";
			std::string s_position = "].position";
			std::string s_constant = "].constant";
			std::string s_linear = "].linear";
			std::string s_quadratic = "].quadratic";
			std::string s_ambient = "].ambient";
			std::string s_diffuse = "].diffuse";
			std::string s_specular = "].specular";
			for (unsigned int i = 0; i < point_lights.size(); i++)
			{
				SomeEntities.setVec3(s_lights + std::to_string(i) +  s_position, point_lights[i].position);
				SomeEntities.setFloat(s_lights + std::to_string(i) +  s_constant, point_lights[i].constant);
				SomeEntities.setFloat(s_lights + std::to_string(i) + s_linear, point_lights[i].linear);
				SomeEntities.setFloat(s_lights + std::to_string(i) + s_quadratic, point_lights[i].quadratic);
				SomeEntities.setVec3(s_lights + std::to_string(i) + s_ambient, point_lights[i].ambient);
				SomeEntities.setVec3(s_lights + std::to_string(i) + s_diffuse, point_lights[i].diffuse);
				SomeEntities.setVec3(s_lights + std::to_string(i) + s_specular, point_lights[i].specular);
			}
		}
		glm::vec3 GetDirLightDirection()
		{
			return parallel_light.direction;
		}

		unsigned int numOfPointLight()
		{
			return point_lights.size();
		}

		PointLight* getPointLightAt(unsigned int idx)
		{
			return &(point_lights[idx]);
		}

		DirLight* getDirectionLight()
		{
			return &parallel_light;
		}
	};
}
#endif // !LIGHT_H

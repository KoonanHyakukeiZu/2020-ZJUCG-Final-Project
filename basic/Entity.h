#ifndef ENTITY_H
#define ENTITY_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>

namespace KooNan
{
	class Entity
	{
	private:
		Shader& EntityShader;
	public:
		Entity(Shader& SomeShader):EntityShader(SomeShader){}
		void Draw(Mesh& mesh,Camera& cam, glm::vec4 clippling_plane, glm::mat4 model, bool if_hit = false)
		{
			EntityShader.use();
			if (if_hit)
				EntityShader.setVec3("selected_color", glm::vec3(0.5f, 0.5f, 0.5f));
			else
				EntityShader.setVec3("selected_color", glm::vec3(0.0f, 0.0f, 0.0f));
			glm::mat4 projection = Common::GetPerspectiveMat(cam);
			EntityShader.setMat4("projection", projection);
			EntityShader.setMat4("view", cam.GetViewMatrix());
			EntityShader.setVec4("plane", clippling_plane);
			EntityShader.setVec3("viewPos", cam.Position);
			EntityShader.setMat4("model", model);
			mesh.Draw(&EntityShader);
		}
		void Pick(Mesh& mesh, Shader& pickingShader, Camera& cam, glm::mat4 model, unsigned int objIndex, unsigned int drawIndex)
		{
			glm::mat4 projection = Common::GetPerspectiveMat(cam);
			pickingShader.use();
			pickingShader.setMat4("projection", projection);
			pickingShader.setMat4("view", cam.GetViewMatrix());
			pickingShader.setMat4("model", model);
			pickingShader.setUint("drawIndex", drawIndex);
			pickingShader.setUint("objIndex", objIndex);
			mesh.Draw(&pickingShader);
		}
	};
}
#endif // !ENTITY_H

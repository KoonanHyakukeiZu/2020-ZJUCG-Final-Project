#pragma once
#include <glm/glm.hpp>
#include <Shader.h>
#include <FileSystem.h>
#include <Model.h>

#include <vector>

namespace KooNan
{
	class GameObject
	{
		// 全局变量
	public:
		static std::vector<GameObject*> gameObjList; // 储存所有游戏物体
	public:
		glm::mat4 modelMat;
	private:
		Model* model;
	public:
		GameObject(const std::string& modelPath, const glm::mat4& modelMat = glm::mat4(1.0f))
		{
			this->modelMat = modelMat;
			if (Model::modelList.find(FileSystem::getPath(modelPath)) == Model::modelList.end())
			{
				// 该模型未加载到List
				Model* model = new Model(FileSystem::getPath(modelPath));
				Model::modelList[modelPath] = model;
				this->model = model;
			}
			else
				// 模型已加载
				this->model = Model::modelList[FileSystem::getPath(modelPath)];
			gameObjList.push_back(this);
		}
		~GameObject()
		{

		}
		void Draw(Shader& shader, const glm::mat4& projectionMat,
			const glm::mat4& viewMat = glm::mat4(1.0f))
		{
			shader.use();
			shader.setMat4("projection", projectionMat);
			shader.setMat4("view", viewMat);
			shader.setMat4("model", modelMat);
			model->Draw(shader);
		}
	};

	std::vector<GameObject*> GameObject::gameObjList;
}
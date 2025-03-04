#pragma once

#include "Model.h"
#include <unordered_map>


class ResourceManager
{
	private:
		std::unordered_map<std::string, Model> models;
		std::unordered_map<std::string, Texture> textures;
		std::unordered_map<std::string, Shader> shaders;

	public:
		ResourceManager();

		void addModel(std::string name, Model &model);
		void addTexture(std::string name, Model &texture);
		void addShader(std::string name, Model &shader);

};

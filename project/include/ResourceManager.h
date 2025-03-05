#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>

#include "Shader.h"
#include "Texture.h"
#include "TTF.h"
#include "Model.h"

class ResourceManager
{
	private:
		std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
		std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
		std::unordered_map<std::string, std::unique_ptr<TTF>> fonts;
		std::unordered_map<std::string, std::unique_ptr<Model>> models;

	public:
		ResourceManager() = default;
		~ResourceManager() = default;
		
		void loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
		Shader* getShader(const std::string& name) const;

		void loadTexture(const std::string& name, const std::string& path, bool isJPG = false);
		Texture* getTexture(const std::string& name) const;

		void loadFont(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& fontPath);
		TTF* getFont(const std::string& name) const;

		void loadModel(const std::string& name, Shader* shader, const std::string& modelPath);
		void loadModel(const std::string& name, Shader* shader, Texture* texture, const std::string& modelPath);
		Model* getModel(const std::string& name) const;
};

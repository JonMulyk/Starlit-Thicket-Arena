#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

void ResourceManager::addModel(std::string name, Model& model)
{
	models.emplace(name, model);
}

void ResourceManager::addTexture(std::string name, Model& texture)
{
	textures.emplace(name, texture);
}

void ResourceManager::addShader(std::string name, Model& shader)
{
	shaders.emplace(name, shader);
}


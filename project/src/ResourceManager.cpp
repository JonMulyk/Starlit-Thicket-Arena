#include "ResourceManager.h"

void ResourceManager::loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath)
{
	if (shaders.find(name) != shaders.end())
	{
		std::cerr << "Shader: " << name << " already loaded\n";
		return;
	}

	shaders[name] = std::make_unique<Shader>(name, vertPath.c_str(), fragPath.c_str());
}

Shader* ResourceManager::getShader(const std::string& name) const
{
	std::unordered_map<std::string, std::unique_ptr<Shader>>::const_iterator shader = shaders.find(name);
	if (shader == shaders.end())
	{
		std::cerr << "Shader: " << name << " not found\n";
		return nullptr;
	}

	return shader->second.get();
}

void ResourceManager::loadTexture(const std::string& name, const std::string& path, bool isJPG)
{
	if (textures.find(name) != textures.end())
	{
		std::cerr << "Texture: " << name << " already loaded\n";
		return;
	}

	textures[name] = std::make_unique<Texture>(path, isJPG);
}

Texture* ResourceManager::getTexture(const std::string& name) const
{
	std::unordered_map<std::string, std::unique_ptr<Texture>>::const_iterator texture = textures.find(name);
	if (texture == textures.end())
	{
		std::cerr << "Texture: " << name << " not found\n";
		return nullptr;
	}

	return texture->second.get();
}

void ResourceManager::loadFont(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& fontPath)
{
	if (fonts.find(name) != fonts.end())
	{
		std::cerr << "TTF: " << name << " already loaded\n";
		return;
	}

	fonts[name] = std::make_unique<TTF>(vertPath.c_str(), fragPath.c_str(), fontPath.c_str());
}

TTF* ResourceManager::getFont(const std::string& name) const
{
	std::unordered_map<std::string, std::unique_ptr<TTF>>::const_iterator font = fonts.find(name);
	if (font == fonts.end())
	{
		std::cerr << "TTF: " << name << " not found\n";
		return nullptr;
	}

	return font->second.get();
}

void ResourceManager::loadModel(const std::string& name, Shader* shader, const std::string& modelPath)
{
	if (models.find(name) != models.end())
	{
		std::cerr << "Model: " << name << " already loaded\n";
		return;
	}

	if (!shader)
	{
		std::cerr << "ERROR: Shader is null for model " << name << std::endl;
		return;
	}

	models[name] = std::make_unique<Model>(*shader, modelPath);
}

void ResourceManager::loadModel(const std::string& name, Shader* shader, Texture* texture, const std::string& modelPath)
{
	if (models.find(name) != models.end())
	{
		std::cerr << "Model: " << name << " already loaded\n";
		return;
	}

	if (!shader)
	{
		std::cerr << "ERROR: Shader is null for model " << name << std::endl;
		return;
	}

	if (!texture)
	{
		std::cerr << "ERROR: Texture is null for model " << name << std::endl;
		return;
	}

	models[name] = std::make_unique<Model>(*shader, *texture, modelPath);
}

Model* ResourceManager::getModel(const std::string& name) const
{
	std::unordered_map<std::string, std::unique_ptr<Model>>::const_iterator model = models.find(name);
	if (model == models.end())
	{
		std::cerr << "Model: " << name << " not found\n";
		return nullptr;
	}

	return model->second.get();
}

void ResourceManager::initializeResources()
{
	// load shaders
    this->loadShader("basicShader", "project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    this->loadShader("lightingShader", "project/assets/shaders/lightingShader.vert", "project/assets/shaders/lightingShader.frag");
    this->loadShader("sceneShader", "project/assets/shaders/CameraShader.vert", "project/assets/shaders/FragShader.frag");
    this->loadShader("skyboxShader", "project/assets/shaders/skyboxShader.vert", "project/assets/shaders/skyboxShader.frag");
	
	// textures

    //this->loadTexture("container", "project/assets/textures/container.jpg", true);
	//this->loadTexture("gold", "project/assets/textures/gold.jpg", true);
	//this->loadTexture("neon", "project/assets/textures/neon.jpg", true);
	//this->loadTexture("fire", "project/assets/textures/fire.jpg", true);
	
	// font
    this->loadFont("arial", "project/assets/shaders/textShader.vert", "project/assets/shaders/textShader.frag", "project/assets/fonts/Arial.ttf");
	
	//models
	/*
    loadModel("redBrick", this->getShader("lightingShader"), this->getTexture("gold"), "project/assets/models/box.obj");
	loadModel("trail", this->getShader("basicShader"), this->getTexture("fire"), "project/assets/models/Trail.obj");
	loadModel("tireModel", this->getShader("basicShader"), "project/assets/models/tire1/tire1.obj");
	*/
}

#pragma once
#include <vector>
#include <string>
#include "Shader.h"

class Skybox
{
	private:
		std::string path;
		Shader& skyboxShader;
		std::vector<std::string> faces;
		unsigned int skyboxVAO;
		unsigned int skyboxVBO;
		unsigned int cubemapTexture;

	public:
		Skybox(std::string path, Shader& skyboxShader);
	
		Shader& getSkyboxShader();
		const unsigned int getSkyboxVAO();
		const unsigned int getCubemapTexture();
		

		unsigned int loadCubeMap(std::vector<std::string> faces);
		void render(Shader& skyboxShader);
};

#pragma once

#include <glm.hpp>
#include <map>
#include "boilerplate/Shader.h"

struct Character
{
	unsigned int textID;	// ID handle of texture
	glm::ivec2 size;		// Size of Glyph	
	glm::ivec2 bearing;		// Offset from baseline
	unsigned int advance;	// Offset to next Glyph
};

std::map<char, Character> initFont(const char* font);
void RenderText(
	Shader& s, 
	unsigned int VAO, 
	unsigned int VBO, 
	std::string text, 
	float x, 
	float y, 
	float scale, 
	glm::vec3 color, 
	std::map<char, Character> 
	characters
);

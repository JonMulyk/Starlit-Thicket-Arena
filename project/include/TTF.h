#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <glm.hpp>
#include "Shader.h"

struct Character {
	unsigned int textureID;	// ID handle of texture
	glm::ivec2 size;		// Size of Glyph	
	glm::ivec2 bearing;		// Offset from baseline
	unsigned int advance;	// Offset to next Glyph
};

class TTF 
{
	public:
		TTF(const char* vert_path, const char* frag_path, const char* font_path, int windowWidth, int windowHeight);

		enum class TEXT_POSITION
		{
			CENTER,
			LEFT,
			RIGHT
		};

		void render(std::string text, float x, float y, float scale, glm::vec3 color, TEXT_POSITION textPosition = TEXT_POSITION::CENTER);

	private:
		Shader m_shader;
		std::map<char, Character> fontLUT;

		GLuint VAO;
		GLuint VBO;

		static std::map<char, Character> createFontLookup(const char* font);
		float getInitalXDrawPosition(std::string text, float x, TEXT_POSITION textPosition, float scale);


};


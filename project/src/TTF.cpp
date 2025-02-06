#include "TTF.h"

std::map<char, Character> TTF::createFontLookup(const char* font) {
    std::map<char, Character> LUT;

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        // TODO: custom exception
        throw std::exception("ERROR::FREETYPE: Could not init FreeType Library");
    }

    FT_Face face;
    if (FT_New_Face(ft, font, 0, &face)) {
        // TODO: custom exception
        throw std::exception("ERROR::FREETYPE: Failed to load font");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //Initialize characters
    for (unsigned char c = 0; c < 128; c++) {

        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Create Texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Create a character struct
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };

        // Add to the lookup table
        LUT.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return LUT;
}

TTF::TTF(const char* vert_path, const char* frag_path, const char* font_path) : m_shader(vert_path, frag_path) {
    fontLUT = createFontLookup(font_path);

    // Initialize the vertex array and buffer
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Cast the text projection matrix
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(1440), 0.0f, static_cast<float>(1440));
    m_shader.use();
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));
}

void TTF::render(std::string text, float x, float y, float scale, glm::vec3 color) {
    // activate corresponding render state	
    m_shader.use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniform3f(glGetUniformLocation(m_shader, "textColor"), color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = fontLUT[*c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#include "TextManager.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

TextManager::TextManager(Shader textShader, const std::string& fontPath) : shader(textShader)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        throw std::runtime_error("Could not init FreeType Library");
    }

    
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        throw std::runtime_error("Failed to load font");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);  // Set size to load glyphs as
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // Disable byte-alignment restriction

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
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
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextManager::~TextManager() {
    // Clean up resources (e.g., textures)
    for (const auto& pair : Characters) {
        glDeleteTextures(1, &pair.second.TextureID);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}



void TextManager::renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    shader.use();  // Use the shader program
    shader.setVec3("textColor", color);  // Set the text color

    glActiveTexture(GL_TEXTURE0);  // Activate the texture
    glBindVertexArray(VAO);       // Bind the VAO

    // Iterate through each character in the text
    for (char c : text) {
        Character ch = Characters[c];  // Get the character data

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f },
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);  // Bind the character texture
        glDrawArrays(GL_TRIANGLES, 0, 6);  // Draw the character quad

        // Advance to the next character
        x += (ch.Advance >> 6) * scale;  // Shift by 6 to convert from 1/64 pixels to pixels
    }

    glBindVertexArray(0);  // Unbind VAO
    glBindTexture(GL_TEXTURE_2D, 0);   // Unbind texture
}

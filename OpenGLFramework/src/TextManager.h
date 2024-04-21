#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <glad/glad.h>
#include <glm.hpp>
#include <map>
#include <string>
#include "Shader.h"

// Struct to hold information about a character glyph
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Horizontal offset to advance to next glyph
};

// Class to encapsulate text rendering logic
class TextManager {
public:
    TextManager(Shader textShader, const std::string& fontPath);
    ~TextManager();  // Destructor to clean up resources

    // Render text at specified coordinates
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
    Shader shader;

private:
    std::map<GLchar, Character> Characters;  // Map of characters
    unsigned int VAO, VBO;                    // VAO and VBO for text rendering
    glm::mat4 projection;                     // Projection matrix for text
};

#endif // TEXTMANAGER_H

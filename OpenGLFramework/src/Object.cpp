#include "Object.h"
#include <vector>


Object::Object(const std::string& vertexPath, const std::string& fragmentPath, const std::string& texturePath1, const std::string& texturePath2, std::vector<float> vertices) {
    // Build and compile the shader program
    shader = new Shader(vertexPath.c_str(), fragmentPath.c_str());
    
    // Generate vertex array object and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind vertex array object
    glBindVertexArray(VAO);

    // Bind and set vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load textures
    textureManager.loadTexture(texturePath1);
    textureManager.loadTexture(texturePath2);
    shader->use();
    shader->setInt("texture1", 0);
    shader->setInt("texture2", 1);
    
}

Object::~Object() {
    // Clean up resources
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete shader;
}

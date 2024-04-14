#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include "Shader.h"
#include "TextureManager.h"
#include <glad/glad.h>
#include "MeshData.h"

class Object {
public:
    Object(const std::string& vertexPath, const std::string& fragmentPath, const std::string& texturePath1, const std::string& texturePath2, std::vector<float> vertices);
    ~Object();

    Shader* shader;
    TextureManager textureManager;
    unsigned int VAO, VBO;
private:    
    
    
    
};

#endif // OBJECT_H

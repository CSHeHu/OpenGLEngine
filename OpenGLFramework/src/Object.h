#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <memory>
#include "Shader.h"
#include "TextureManager.h"
#include <glad/glad.h>
#include "MeshData.h"

class Object {
public:
    Object(const std::string& vertexPath, const std::string& fragmentPath, const std::string& texturePath1, const std::string& texturePath2, const std::vector<float>& vertices, const glm::vec3& position);
    ~Object();
    glm::vec3 getPosition() const;

    std::shared_ptr<Shader> shader;
    TextureManager textureManager;
    unsigned int VAO, VBO;
private:    
    glm::vec3 pos;
    
    
};

#endif // OBJECT_H

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include <vector>

class TextureManager {
public:
    TextureManager();
    void loadTexture(const std::string& path);
    void bindTextures() const;
    

private:
    std::vector<unsigned int> m_TextureIDs;
};

#endif // TEXTURE_MANAGER_H

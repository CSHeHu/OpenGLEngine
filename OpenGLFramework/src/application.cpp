#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <cmath>
#include <vector>
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H  
#include <map>

#include "Shader.h"
#include "Camera.h"
#include "InputManager.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include "InputManager.h"
#include "MeshData.h"
#include "TextureManager.h"
#include "Object.h"



// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float COLLISION_BUFFER = 1.2f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool isCollisionCube(std::vector<std::shared_ptr<Object>>& cubeInstances);
bool isCollisionPyramid(std::vector<std::shared_ptr<Object>>& pyramidInstances);
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int VAO, VBO;
int points = 0;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // callbacks for window resize, mouse movement and scroll movement
    InputManager::setCamera(&camera);
    glfwSetFramebufferSizeCallback(window, InputManager::framebufferSizeCallback);
    glfwSetCursorPosCallback(window, InputManager::mouseCallback);
    glfwSetScrollCallback(window, InputManager::scrollCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // create objects containers
    std::vector<std::shared_ptr<Object>> cubeInstances;
    std::vector<std::shared_ptr<Object>> pyramidInstances;
    
    // create shaders for view and projection
    Shader projectionShader("shaders/projection.vs", "shaders/projection.fs");
    Shader viewShader("shaders/view.vs", "shaders/view.fs");
    Shader textShader("shaders/text.vs", "shaders/text.fs");
    
    
    
    //Create objects
    for (int i = 0; i < cubePositions.size(); ++i) {
        std::shared_ptr<Object> cube = std::make_shared<Object>("shaders/cube.vs", "shaders/cube.fs", "textures/bunny.jpg", "textures/wall.jpg", cubeVertices, cubePositions.at(i));
        cubeInstances.push_back(cube);
    }
    for (int i = 0; i < pyramidPositions.size(); ++i) {
        std::shared_ptr<Object> pyramid = std::make_shared<Object>("shaders/pyramid.vs", "shaders/pyramid.fs", "textures/haisuli.png", "textures/wall.jpg", pyramidVertices, pyramidPositions.at(i));
        pyramidInstances.push_back(pyramid);
    }
    

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // find path to font
    std::string font_name = "textures/Roboto.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
    }
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

    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        if (cubeInstances.size() == 0) {
            glfwSetWindowShouldClose(window, true);
        }

        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----

        InputManager::processInput(window, deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        

        // Set up projection matrix
        projectionShader.use();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        projectionShader.setMat4("projection", projection);

        // Set up view matrix
        viewShader.use();
        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        viewShader.setMat4("view", view);

        // setup text
        textShader.use();
        RenderText(textShader, "Points: " + std::to_string(points), 25.0f, SCR_HEIGHT - 25.0f, 0.5f, glm::vec3(1.0, 1.0f, 1.0f));
        RenderText(textShader, "x", SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f, 0.5f, glm::vec3(1.0, 1.0f, 1.0f));
        glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
        textShader.setMat4("projection", textProjection);
        

        
        // renred objects
        for (int i = 0; i < cubeInstances.size(); ++i) {
            cubeInstances.at(i)->textureManager.bindTextures();
            cubeInstances.at(i)->shader->use();
            cubeInstances.at(i)->shader->setMat4("projection", projection);
            cubeInstances.at(i)->shader->setMat4("view", view);
            glBindVertexArray(cubeInstances.at(i)->VAO);
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubeInstances.at(i)->getPosition());
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle) + (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            cubeInstances.at(i)->shader->setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size() / 5);
            
        }

        for (int i = 0; i < pyramidInstances.size(); ++i) {
            pyramidInstances.at(i)->textureManager.bindTextures();
            pyramidInstances.at(i)->shader->use();
            pyramidInstances.at(i)->shader->setMat4("projection", projection);
            pyramidInstances.at(i)->shader->setMat4("view", view);
            glBindVertexArray(pyramidInstances.at(i)->VAO);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pyramidInstances.at(i)->getPosition());
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle) + (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            pyramidInstances.at(i)->shader->setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, pyramidVertices.size() / 5);
        }



        // check collision
        if (isCollisionCube(cubeInstances)) {
            std::cout << "HIT Cube" << std::flush;
            std::cout << std::endl;

        }

        // check collision
        if (isCollisionPyramid(pyramidInstances)) {
            std::cout << "HIT Pyramid" << std::flush;
            std::cout << std::endl;

        }
        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

bool isCollisionCube(std::vector<std::shared_ptr<Object>>& cubeInstances) {
    // Iterate through each object's position
    for (int i = 0; i < cubeInstances.size(); ++i)
    {
        
        glm::vec3 cubePos = cubeInstances.at(i)->getPosition();

        // Check if camera position intersects with adjusted object position
        if (std::abs(camera.Position.x - cubePos.x) < COLLISION_BUFFER &&
            std::abs(camera.Position.y - cubePos.y) < COLLISION_BUFFER &&
            std::abs(camera.Position.z - cubePos.z) < COLLISION_BUFFER)
        {
            // Collision detected
            cubeInstances.erase(cubeInstances.begin() + i);
            points += 100;
            return true;
        }
    }

    // No collision detected
    return false;
}

bool isCollisionPyramid(std::vector<std::shared_ptr<Object>>& pyramidInstances) {
    for (int i = 0; i < pyramidInstances.size(); ++i)
    {
        glm::vec3 pyramidPos = pyramidInstances.at(i)->getPosition();
        
        // Check if camera is inside the bounding box of the pyramid
        if (std::abs(camera.Position.x - pyramidPos.x) < COLLISION_BUFFER &&
            std::abs(camera.Position.y - pyramidPos.y) < COLLISION_BUFFER &&
            std::abs(camera.Position.z - pyramidPos.z) < COLLISION_BUFFER)
        {
            // Calculate the displacement vector from pyramid to camera
            glm::vec3 displacement = camera.Position - pyramidPos;
            
            // Calculate the intersection point between camera and pyramid
            camera.Position = pyramidPos + glm::normalize(displacement) * (1.05f * COLLISION_BUFFER);
            points -= 10;
            return true;
        }
    }

    return false;
}

// render line of text
// -------------------
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
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
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
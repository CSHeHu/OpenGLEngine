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
    std::vector<std::shared_ptr<Object>> cubeInstances;
    std::vector<std::shared_ptr<Object>> pyramidInstances;
    glEnable(GL_DEPTH_TEST);
    Shader projectionShader("shaders/projection.vs", "shaders/projection.fs");
    Shader viewShader("shaders/view.vs", "shaders/view.fs");
    
    //Create objects
    for (int i = 0; i < cubePositions.size(); ++i) {
        std::shared_ptr<Object> cube = std::make_shared<Object>("shaders/cube.vs", "shaders/cube.fs", "textures/bunny.jpg", "textures/wall.jpg", cubeVertices, cubePositions.at(i));
        cubeInstances.push_back(cube);
    }
    for (int i = 0; i < pyramidPositions.size(); ++i) {
        std::shared_ptr<Object> pyramid = std::make_shared<Object>("shaders/pyramid.vs", "shaders/pyramid.fs", "textures/haisuli.png", "textures/wall.jpg", pyramidVertices, pyramidPositions.at(i));
        pyramidInstances.push_back(pyramid);
    }

    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
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

            return true;
        }
    }

    return false;
}
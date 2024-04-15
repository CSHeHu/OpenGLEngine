#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <cmath>
#include <vector>

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

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

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

    Shader projectionShader("shaders/projection.vs", "shaders/projection.fs");
    Shader viewShader("shaders/view.vs", "shaders/view.fs");
    Object cube("shaders/cube.vs", "shaders/cube.fs", "textures/bunny.jpg", "textures/wall.jpg", cubeVertices);
    Object pyramid("shaders/pyramid.vs", "shaders/pyramid.fs", "textures/haisuli.png", "textures/wall.jpg", pyramidVertices);


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

        //bind cubes textures
        cube.textureManager.bindTextures();
        // activate cubes shader
        cube.shader->use();
        // pass projection matrix to shader (note that in this case it could change every frame)       
        cube.shader->setMat4("projection", projection);
        // camera/view
        cube.shader->setMat4("view", view);

        // render cube object
        glBindVertexArray(cube.VAO);

        for (int i = 0; i < 10; ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions.at(i));
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle) + (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            cube.shader->setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size() / 5);
        }

        pyramid.textureManager.bindTextures();
        pyramid.shader->use();
        pyramid.shader->setMat4("projection", projection);
        pyramid.shader->setMat4("view", view);

        glBindVertexArray(pyramid.VAO);

        for (int i = 0; i < 10; ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pyramidPositions.at(i));
            float angle = 50.0f * i;
            model = glm::rotate(model, glm::radians(angle) + (float)glfwGetTime(), glm::vec3(0.5f, 0.7f, 0.2f));
            pyramid.shader->setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, pyramidVertices.size() / 5);
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
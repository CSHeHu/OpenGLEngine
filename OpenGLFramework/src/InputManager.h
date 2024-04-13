#define GLFW_INCLUDE_NONE
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <GLFW/glfw3.h>

#include "Camera.h"



class InputManager
{
public:
	static void setCamera(Camera* cameraPtr);
	static void processInput(GLFWwindow* window, Camera* camera, float deltaTime);
	
	//glfw: whenever the window size changed (by OS or user resize) this callback function executes
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);


private:
	static float lastX;
	static float lastY;
	static bool firstMouse;
	static Camera* camera;
};



#endif

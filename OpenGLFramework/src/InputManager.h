#define GLFW_INCLUDE_NONE
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <GLFW/glfw3.h>

#include "Camera.h"



class InputManager
{
public:
	InputManager(GLFWwindow* window, Camera* camera);
	void processInput(float deltaTime);

private:
	GLFWwindow* window;
	Camera* camera;
};



#endif

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "RBF.h"



//TODO: make it singleton
class InputManager{

    static RBFcircle* myCircle;
    

public:

    InputManager(RBFcircle*,GLFWwindow*);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

};
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "RBF.h"
#include "defines.h"




//TODO: make it singleton
class InputManager{

    static RBF* myRBF;
    static vec2 mousePos;


public:

    InputManager(RBF*,GLFWwindow*);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);




    static vec2 GetMousePos();
};
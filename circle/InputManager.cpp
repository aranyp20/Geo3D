#include <iostream>
#include "InputManager.h"

RBF* InputManager::myRBF = NULL;
vec2 InputManager::mousePos = vec2(-1,-1);

InputManager::InputManager(RBF* _myRBF,GLFWwindow* _myWindow)
{
    myRBF = _myRBF;
    glfwSetKeyCallback(_myWindow, key_callback);
    glfwSetCursorPosCallback(_myWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(_myWindow,mouse_button_callback);

}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        if(PROGRAM_TYPE==POLYLINE_PROGRAM)static_cast<RBFpolyline*>(myRBF)->AddNode(GetMousePos());
    }
        
}

void InputManager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mousePos = vec2(SIZE-ypos,xpos);
  
}



void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

       
    if(action==GLFW_PRESS){
        switch(key){
            case GLFW_KEY_M:
                if(PROGRAM_TYPE==CIRCLE_PROGRAM)static_cast<RBFcircle*>(myRBF)->IncreaseN();
            break;

            case GLFW_KEY_N:
                if(PROGRAM_TYPE==CIRCLE_PROGRAM)static_cast<RBFcircle*>(myRBF)->DecreaseN();
            break;


        }
       
    }
}


vec2 InputManager::GetMousePos(){return mousePos;}
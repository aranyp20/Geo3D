#include "InputManager.h"

RBFcircle* InputManager::myCircle = NULL;

InputManager::InputManager(RBFcircle* _myCircle,GLFWwindow* _myWindow)
{
    myCircle = _myCircle;
    glfwSetKeyCallback(_myWindow, key_callback);

}


void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

       
    if(action==GLFW_PRESS){
        switch(key){
            case GLFW_KEY_M:
                myCircle->IncreaseN();
            break;

            case GLFW_KEY_N:
                myCircle->DecreaseN();
            break;

        }
       
    }
}
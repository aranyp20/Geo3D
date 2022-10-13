#include <iostream>
#include <algorithm>
#include "bitMapPrinter.h"


MapPrinter::MapPrinter(size_t _size) : size(_size){
    if (!glfwInit())
        return ;


    window = glfwCreateWindow(size, size, "RBF demo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return ;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    glViewport(0,0,size,size);
    glClearColor(1.0f,1.0f,1.0f,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

    glViewport(0, 0, size, size);
    
  
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, false);    



}


BitMapPrinter::BitMapPrinter(size_t _size): MapPrinter(_size)
{
    datat = CreateMap<bool>(size,size);
    
}

FloatMapPrinter::FloatMapPrinter(size_t _size): MapPrinter(_size)
{
    datat = CreateMap<float>(size,size);
    
}



void BitMapPrinter::Print()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    char data2[size][size][3];
    
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            if(datat[i][j]){
                data2[i][j][0]=0;
                data2[i][j][1]=100;
                data2[i][j][2]=0;
                
            }else{
                data2[i][j][0]=0;
                data2[i][j][1]=0;
                data2[i][j][2]=0;
            }
            
            
        }
    } 
    glDrawPixels(size,size,GL_RGB,GL_UNSIGNED_BYTE,data2);
            
    glfwSwapBuffers(window);
}

void FloatMapPrinter::Print()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    char data2[size][size][3];
    
    float floatToToken = 255/0.5f;

    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
           float ratio = 2*datat[i][j];
            data2[i][j][0]=std::max(0.0f, 255*(1 - ratio));
            data2[i][j][2]= std::max(0.0f, 255*(ratio - 1));
            data2[i][j][1]= 255 - data2[i][j][2] - data2[i][j][0];
        }
    } 
    glDrawPixels(size,size,GL_RGB,GL_UNSIGNED_BYTE,data2);
            
    glfwSwapBuffers(window);
}

void BitMapPrinter::RefreshData(const bitmap& _data)
{
    datat = _data;
}

void FloatMapPrinter::RefreshData(const floatmap& _data)
{
    datat = _data;
}

GLFWwindow* MapPrinter::GetWindow(){return window;}

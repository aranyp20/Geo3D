#ifndef __BITMAPPRINTER__
#define __BITMAPPRINTER__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "general.hpp"


class MapPrinter
{
protected:

    GLFWwindow* window;
    size_t size;
    

public:

    MapPrinter(size_t);
    GLFWwindow* GetWindow();

    virtual void Print() = 0;
};


class BitMapPrinter : public MapPrinter
{
    
    bitmap datat;
public:

    BitMapPrinter(size_t);

    void RefreshData(const bitmap&);
    
    void Print() override;
};

class FloatMapPrinter : public MapPrinter
{
    //range of floats: [0-1]
    floatmap datat;
public:

    FloatMapPrinter(size_t);

    void RefreshData(const floatmap&);
    
    void Print() override;
};













#endif
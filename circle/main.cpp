#include <iostream>
#include "bitMapPrinter.h"
#include "RBF.h"
#include "InputManager.h"


class Program{
    MapPrinter* mp;
    RBF* rbf;

    

public:

    void Run()
    {
        

        while(!glfwWindowShouldClose(mp->GetWindow())){

            if(!rbf->isValid()){
                
                mp->RefreshData(rbf);
                mp->Print();
            }
          

            glfwPollEvents();
        }
        glfwTerminate();
    }

    Program()
    {
        switch(PRINTER_TYPE){
            case BITMAP_PRINTER: mp = new BitMapPrinter(SIZE); break;
            case FLOATMAP_PRINTER: mp = new FloatMapPrinter(SIZE); break;
        }

        RBF::KernelStrategy* currStrategy;
        switch(KERNEL_FUNCTION){
            case NORMAL_KERNEl: currStrategy = new RBF::NormalKernel(); break;
            case KOMPAKT_KERNEL: currStrategy = new RBF::KompaktKernel(); break;
        }

        switch(PROGRAM_TYPE){
            case CIRCLE_PROGRAM: rbf = new RBFcircle(SIZE,2,20,currStrategy); break;
            case POLYLINE_PROGRAM: rbf = new RBFpolyline(SIZE,20,currStrategy); break;
        }
        InputManager im(rbf,mp->GetWindow());

        
        Run();
    }


};


int main()
{
 

    Program program;


    return 0;
}

#include <iostream>
#include "bitMapPrinter.h"
#include "RBF.h"
#include "InputManager.h"


class Program{
    FloatMapPrinter* bmp;
    RBF* rbf;

public:

    void Run()
    {
        

        while(!glfwWindowShouldClose(bmp->GetWindow())){

            if(!rbf->isValid()){

                bmp->RefreshData(rbf->EvaluateAllRaw());
            
                bmp->Print();
            }
          

            glfwPollEvents();
        }
        glfwTerminate();
    }

    Program():bmp(new FloatMapPrinter(SIZE))
    {
        switch(PROGRAM_TYPE){
            case CIRCLE_PROGRAM: rbf = new RBFcircle(SIZE,2,20); break;
            case POLYLINE_PROGRAM: rbf = new RBFpolyline(SIZE,20); break;
        }
        InputManager im(rbf,bmp->GetWindow());
        Run();
    }


};


int main()
{
 

    Program program;


    return 0;
}
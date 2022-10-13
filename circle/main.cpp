#include <iostream>
#include "bitMapPrinter.h"
#include "RBF.h"
#include "InputManager.h"


#define SIZE 500

class Program{
    FloatMapPrinter* bmp;
    RBFcircle* rbf;

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

    Program():bmp(new FloatMapPrinter(SIZE)), rbf(new RBFcircle(SIZE,2,20))
    {
        InputManager im(rbf,bmp->GetWindow());
        Run();
    }


};


int main()
{


    Program program;


    return 0;
}
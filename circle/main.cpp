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
        RBF::KernelStrategy* tKStrategy;
        switch(KERNEL_FUNCTION){
            case NORMAL_KERNEl: tKStrategy = new RBF::NormalKernel(); break;
            case KOMPAKT_KERNEL: tKStrategy = new RBF::KompaktKernel(); break;
        }
        RBF::CoefficientStrategy* tCStrategy;
        switch(COEFFICIENT_CALC_TYPE){
            case NORMAL_COEFFICIENT_CALC: tCStrategy = new RBF::NormalCoefficient();break;
            case ITERATIVE_COEFFICIENT_CALC: tCStrategy = new RBF::IterativeCoefficient();break;
        }
        switch(PROGRAM_TYPE){
            case CIRCLE_PROGRAM: rbf = new RBFcircle(SIZE,2,20,RBF::StrategyPack{tKStrategy,tCStrategy}); break;
            case POLYLINE_PROGRAM: rbf = new RBFpolyline(SIZE,20,RBF::StrategyPack{tKStrategy,tCStrategy}); break;
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

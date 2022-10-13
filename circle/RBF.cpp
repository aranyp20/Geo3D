#include <iostream>

#include "RBF.h"


RBF::RBF(size_t _size, float _h) : size(_size),currentH(_h){}

RBFcircle::NodeExtra::NodeExtra(const vec2& _node,float _value) : node(_node), value(_value)
{
}

RBFcircle::RBFcircle(size_t _size, unsigned int _n, float _h) : RBF(_size,_h),circleCenter(_size/2,_size/2), radius(_size/5),currentN(_n)
{
    Calibrate(_n,_h);
    bResult = CreateMap<bool>(size,size);
    fResult = CreateMap<float>(size,size);
}

void RBFcircle::Calibrate(unsigned int _n, float _h)
{
    nodexs.clear();

    for(int i=0;i<_n;i++){
        float alpha = (M_PI * 2 * i)/_n;
        vec2 centerNode(circleCenter.x+radius*cosf(alpha),circleCenter.y+radius*sinf(alpha));
        nodexs.push_back(NodeExtra(centerNode,0));
        vec2 normal = (centerNode-circleCenter).normalize();
        nodexs.push_back(NodeExtra(centerNode+normal*_h,_h));
        nodexs.push_back(NodeExtra(centerNode-normal*_h,-_h));
    }

    calculateCoefficients();
}

void RBF::calculateCoefficients()
{
    float sh = nodexs.size();
    Eigen::MatrixXf A = Eigen::MatrixXf::Random(sh, sh);
    for(int i=0;i<sh;i++){
        for(int j=0;j<sh;j++){
            A(i,j) = cFun(nodexs[i].node,nodexs[j].node);
        }
    }
    Eigen::VectorXf b = Eigen::VectorXf::Random(sh);
    Eigen::VectorXf c = Eigen::VectorXf::Random(sh);
    for(int i=0;i<sh;i++){
        c(i) = nodexs[i].value;
    }
   
    b = A.colPivHouseholderQr().solve(c);
 
    for(int i=0;i<sh;i++){
        nodexs[i].coefficient = b[i]; 
        
    }
}

float RBF::cFun(const vec2& p1, const vec2& p2)
{
    float preferableConstant = 5.0f;
    float distance = (p1-p2).length();

    return sqrt(distance*distance + preferableConstant * preferableConstant);
}


float RBF::EvaluateRaw(float _x, float _y)
{
    float tval = 0.0f;

    for(int i=0;i<nodexs.size();i++){
        tval += nodexs[i].coefficient * cFun(vec2(_x,_y),nodexs[i].node);     
    }

    return tval;
}

bool RBF::Evaluate(float _x, float _y)
{
    
    if(EvaluateRaw(_x,_y)>30)return false;
    return true;
}

bitmap RBF::EvaluateAll()
{
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            bResult[i][j] = Evaluate(i,j);
           
        }
    }

    validate();
    return bResult;
}

floatmap RBF::EvaluateAllRaw()
{
    float minVal = EvaluateRaw(0,0);
    float maxVal = EvaluateRaw(0,0);
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            fResult[i][j] = EvaluateRaw(i,j);
            if(minVal>fResult[i][j])minVal = fResult[i][j];
            if(maxVal<fResult[i][j])maxVal = fResult[i][j];
        }
    }

    /* for(int i=1;i<250;i++){
        if(fResult[i][250]>fResult[i-1][250]){
            std::cout<<i<<std::endl;
            break;
        }
    } */


    //normalizing values
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            fResult[i][j] -= minVal;
            fResult[i][j] /= maxVal-minVal;
        }
    }

    validate();
    return fResult;
}

void RBF::AddNode(const vec2& _pos, float _value)
{

    invalidate();
}

void RBFcircle::IncreaseN()
{
    Calibrate(++currentN,currentH);
    std::cout<<"Number of nodes: "<<currentN<<" ("<<currentN*3<<")"<<std::endl;
    invalidate();
}

void RBFcircle::DecreaseN()
{
    if(currentN<=2)return;
    Calibrate(--currentN,currentH);
    std::cout<<"Number of nodes: "<<currentN<<" ("<<currentN*3<<")"<<std::endl;
    invalidate();
}
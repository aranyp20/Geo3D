#include <iostream>
#include <limits>

#include "RBF.h"
#include "defines.h"


RBF::RBF(size_t _size, float _h,const StrategyPack& sp) : size(_size),currentH(_h),startingH(_h),myKernel(sp.ks),myCoefficent(sp.cs)
{

    bResult = CreateMap<bool>(size,size);
    fResult = CreateMap<float>(size,size);
}

RBF::NodeExtra::NodeExtra(const vec2& _node,float _value,const vec2& _norm) : node(_node), value(_value), normal(_norm)
{
}

RBFcircle::RBFcircle(size_t _size, unsigned int _n, float _h,const StrategyPack& sp) : RBF(_size,_h,sp),circleCenter(_size/2,_size/2), radius(_size/5),currentN(_n),startingN(_n)
{
    Calibrate(_n);
    
}

void RBFcircle::Calibrate(unsigned int _n)
{
    nodexs.clear();

    for(int i=0;i<_n;i++){
        float alpha = (M_PI * 2 * i)/_n;
        vec2 centerNode(circleCenter.x+radius*cosf(alpha),circleCenter.y+radius*sinf(alpha));
        vec2 tNormal = (centerNode-circleCenter).normalize();
        AddNodeHelper(centerNode,tNormal,false);
    }

    calculateCoefficients();
}

//expects "kompakt" kernel
vec2 RBF::NodeExtra::calcGrad(const vec2& where) const
{
    float normalizedDistance = (where-node).length() / NODE_INFLUENCE_ZONE;
    if(normalizedDistance>1 || normalizedDistance < 0.0001)return vec2(0,0);

    return vec2(((where.x-node.x)/normalizedDistance) * (-4*powf(1-normalizedDistance,3)*(4*normalizedDistance + 1)+powf(1-normalizedDistance,4)*4),((where.y-node.y)/normalizedDistance) * (-4*powf(1-normalizedDistance,3)*(4*normalizedDistance + 1)+powf(1-normalizedDistance,4)*4));

}

void RBF::calculateCoefficients()
{
    myCoefficent->CalculateCoefficients(*this);
}

void RBF::NormalCoefficient::CalculateCoefficients(RBF& r)
{
    unsigned int sh = r.nodexs.size();
    Eigen::MatrixXf A = Eigen::MatrixXf::Random(sh, sh);
    for(int i=0;i<sh;i++){
        for(int j=0;j<sh;j++){
            A(i,j) = r.cFun(r.nodexs[i].node,r.nodexs[j].node);
        }
    }
    Eigen::VectorXf b = Eigen::VectorXf::Random(sh);
    Eigen::VectorXf c = Eigen::VectorXf::Random(sh);
    for(int i=0;i<sh;i++){
        c(i) = r.nodexs[i].value;
    }
   
    b = A.colPivHouseholderQr().solve(c);
 
    for(int i=0;i<sh;i++){
        r.nodexs[i].coefficient = b[i]; 
        
    }
}

void RBF::IterativeCoefficient::CalculateCoefficients(RBF& r)
{
    unsigned int sh = r.nodexs.size();


    for(int i=0;i<sh;i++){
        float startValue = 0;
        for(int j=0;j<sh;j++){
            startValue += dot(r.nodexs[i].normal,r.nodexs[j].calcGrad(r.nodexs[i].node));  
        }
        startValue *=-1;
        r.nodexs[i].startValueSave = startValue;
        r.nodexs[i].coefficient = startValue;
    }

    float maxDiffVec = std::numeric_limits<float>::max();
    for(int k = 0;k<MAX_ITERATION && fabs(maxDiffVec)>MAX_ERROR;k++){
        for(int i=0;i<sh;i++){

            float tValHere = 0;
            for(int j=0;j<sh;j++){
                tValHere += r.nodexs[j].coefficient * r.cFun(r.nodexs[i].node,r.nodexs[j].node);
            }
            float diffVec = r.nodexs[i].startValueSave - tValHere;

            if(fabs(diffVec)<fabs(maxDiffVec))maxDiffVec = diffVec;

            r.nodexs[i].coefficientNext = r.nodexs[i].coefficient + diffVec;
        }

        for(int i=0;i<sh;i++){
            r.nodexs[i].coefficient = r.nodexs[i].coefficientNext;
        }
    }


}

float RBF::cFun(const vec2& p1,const vec2& p2) const
{
   return myKernel->KernelFun(p1,p2);
}



float const RBF::NormalKernel::KernelFun(const vec2& p1, const vec2& p2)
{
    float preferableConstant = 5.0f;
    float distance = (p1-p2).length();

    return sqrt(distance*distance + preferableConstant * preferableConstant);
}

float const RBF::KompaktKernel::KernelFun(const vec2& p1, const vec2& p2)
{
    float influenceZone = NODE_INFLUENCE_ZONE;
    float normalizedDistance = (p1-p2).length() / influenceZone;
    if(normalizedDistance<0 || normalizedDistance>1) return 0;
    return powf(1-normalizedDistance,4) * (4 * normalizedDistance + 1);
}

float RBF::EvaluateRaw(float _x, float _y) const
{
    float tval = 0.0f;
    float sh = nodexs.size();

    /* for(int i=0;i<nodexs.size();i++){
        tval += nodexs[i].coefficient * cFun(vec2(_x,_y),nodexs[i].node);     
    } */

    

    float left = 0;
    float right = 0;
    for(int i=0;i<sh;i++){
        left += nodexs[i].coefficient * cFun(nodexs[i].node,vec2(_x,_y));
        right += dot(nodexs[i].normal,nodexs[i].calcGrad(vec2(_x,_y)));
    }

    

    return left + right;
    //return tval;
}

bool RBF::Evaluate(float _x, float _y) const
{
    
    if(EvaluateRaw(_x,_y)>10)return false;
    return true;
}

bitmap RBF::EvaluateAll()
{
    if(nodexs.size()<2)return bResult;

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

void RBFpolyline::AddNode(const vec2& _pos)
{
    cps.push_back(_pos);
    Calibrate();
    invalidate();
}

void RBFpolyline::Calibrate()
{
    nodexs.clear();
    unsigned int cSize = cps.size();
    if(cSize<2)return;
    vec2 tNormal = (cps[1]-cps[0]).normalize().rotate();
    AddNodeHelper(cps[0],tNormal,false);
    
    for(int i = 1;i<cSize-1;i++){

        tNormal = (cps[i+1]-cps[i-1]).normalize().rotate();
        /* vec2 tVec = ((((cps[i-1]-cps[i]).normalize() * 0.5f) + ((cps[i+1]-cps[i]).normalize() * 0.5f)) );
        if( FE(tVec.length(),0,0.1f))tNormal = (cps[i-1]-cps[i]).normalize().rotate();
        else{tNormal = tVec.normalize();}*/
        AddNodeHelper(cps[i],tNormal,false); 

    }

    tNormal = (cps[cSize-1]-cps[cSize-2]).normalize().rotate();
    
    AddNodeHelper(cps[cSize-1],tNormal,false);

    calculateCoefficients();
}

void RBF::AddNodeHelper(const vec2& _pos,const vec2& _normal, bool isLine)
{
    if(COEFFICIENT_CALC_TYPE == NORMAL_COEFFICIENT_CALC){
        int tSign = isLine ? 1 : -1;
        nodexs.push_back(NodeExtra(_pos,1));
        nodexs.push_back(NodeExtra(_pos+_normal*currentH,currentH));
        nodexs.push_back(NodeExtra(_pos-_normal*currentH,currentH*tSign));
    }else if(COEFFICIENT_CALC_TYPE == ITERATIVE_COEFFICIENT_CALC){
        nodexs.push_back(NodeExtra(_pos,1,_normal));
    }

}

void RBFcircle::IncreaseN()
{
    Calibrate(++currentN);
    invalidate();
    std::cout<<"Number of nodes: "<<currentN<<" ("<<currentN*3<<")"<<std::endl;
}

void RBFcircle::DecreaseN()
{
    if(currentN<=2)return;
    Calibrate(--currentN);
    invalidate();
    std::cout<<"Number of nodes: "<<currentN<<" ("<<currentN*3<<")"<<std::endl;
}

void RBF::IncreaseH()
{
    currentH++;
    invalidate();
    ReCalibrate();
    std::cout<<"Current node distance: "<<currentH<<std::endl;
}

void RBF::DecreaseH()
{
    if(currentH<=1)return;
    currentH--;
    ReCalibrate();
    invalidate();
    std::cout<<"Current node distance: "<<currentH<<std::endl;
}

void RBFcircle::ReCalibrate()
{
    Calibrate(currentN);
}

void RBFpolyline::ReCalibrate()
{

    Calibrate();
}

void RBF::Reset()
{
    bResult = CreateMap<bool>(size,size,false);
    fResult = CreateMap<float>(size,size,100);
    nodexs.clear();
    currentH = startingH;
    ReCalibrate();
    invalidate();
}

void RBFcircle::Reset()
{
    currentN = startingN;
    RBF::Reset();
}

void RBFpolyline::Reset()
{
    cps.clear();
    RBF::Reset();
}

RBFpolyline::RBFpolyline(size_t _size,float _h,const StrategyPack& sp) : RBF(_size,_h,sp)
{
    
}

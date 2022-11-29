#include <iostream>
#include <limits>

#include "RBF.h"
#include "defines.h"


RBF::RBF(size_t _size, double _h,const StrategyPack& sp) : size(_size),currentH(_h),startingH(_h),myKernel(sp.ks),myCoefficent(sp.cs)
{

    bResult = CreateMap<bool>(size,size);
    fResult = CreateMap<double>(size,size);
}

RBF::NodeExtra::NodeExtra(const vec2& _node,double _value,size_t _index,const vec2& _norm) : node(_node), value(_value), index(_index),normal(_norm)
{

}

RBFcircle::RBFcircle(size_t _size, unsigned int _n, double _h,const StrategyPack& sp) : RBF(_size,_h,sp),circleCenter(_size/2,_size/2), radius(_size/5),currentN(_n),startingN(_n)
{
    Calibrate(_n);
    
}

void RBFcircle::Calibrate(unsigned int _n)
{
    nodexs.clear();

    for(int i=0;i<_n;i++){
        double alpha = (M_PI * 2 * i)/_n;
        vec2 centerNode(circleCenter.x+radius*cosf(alpha),circleCenter.y+radius*sinf(alpha));
        vec2 tNormal = (centerNode-circleCenter).normalize();
        AddNodeHelper(centerNode,tNormal,false);
    }

    calculateCoefficients();
}

vec2 RBF::CalcGradGood(const RBF::NodeExtra& n, const vec2& p)const
{
    double eps = 1e-10;
    return vec2(((cFun(n,p+vec2(eps,0))-cFun(n,p))/eps),((cFun(n,p+vec2(0,eps))-cFun(n,p))/eps));
}

////////////////////////////////////////////


vec2 RBF::CalcGrad(const NodeExtra& n, const vec2& p) const
{
    std::vector<vec2> poss;
    for(const auto& a : nodexs){
        poss.push_back(a.node);
    }
    vec2 m = CalcGradGood(n,p);



    return m;


}
double phiD(double r)
{
    return ((20*r)/(NODE_INFLUENCE_ZONE*NODE_INFLUENCE_ZONE))*(std::pow((r/NODE_INFLUENCE_ZONE)-1,3));
}

vec2 RBF::NodeExtra::calcGrad(const vec2& where) const
{
    double normalizedDistance = (where-node).length() / NODE_INFLUENCE_ZONE;
    double distance = (where-node).length();
    if(normalizedDistance>1 || normalizedDistance < 1e-10)return vec2(0,0);


    return vec2((((where.x-node.x)/normalizedDistance)*phiD(distance)),(((where.y-node.y)/normalizedDistance)*phiD(distance)));
}


double const RBF::KompaktKernel::KernelFun(const vec2& p1, const vec2& p2)
{

    double influenceZone = NODE_INFLUENCE_ZONE;
    double normalizedDistance = (p1-p2).length() / influenceZone;
    

    if(normalizedDistance>1) return 0;
    

    return std::pow(1-normalizedDistance,4) * (4 * normalizedDistance + 1);
}

double RBF::EvaluateRaw(double _x, double _y) const
{
    

    double tval = 0.0f;
    double sh = nodexs.size();
    //CALC_COEFFICIENT_TYPE
    /* for(int i=0;i<nodexs.size();i++){
        tval += nodexs[i].coefficient * cFun(vec2(_x,_y),nodexs[i].node);     
    } */

    

    double left = 0;
    double right = 0;
    for(int i=0;i<sh;i++){
        left += nodexs[i].coefficient  *cFun(nodexs[i],vec2(_x,_y));
        right += dot(nodexs[i].normal,CalcGrad(nodexs[i],vec2(_x,_y)));
    }
    
    

    return right+left;
    //return tval;
}


void RBF::calculateCoefficients()
{
    myCoefficent->CalculateCoefficients(*this);
}



void RBF::IterativeCoefficient::CalculateCoefficients(RBF& r)
{

    unsigned int sh = r.nodexs.size();


    for(int i=0;i<sh;i++){
        double startValue = 0;
        for(int j=0;j<sh;j++){
            startValue += dot(r.nodexs[j].normal,r.CalcGrad(r.nodexs[j],r.nodexs[i].node));  
        }
        startValue *=-1;
        r.nodexs[i].startValueSave = startValue;
        r.nodexs[i].coefficient = startValue;


    }

    double maxDiffVec = std::numeric_limits<double>::max();
    for(int k = 0;k<MAX_ITERATION && abs(maxDiffVec)>MAX_ERROR;k++){
        for(int i=0;i<sh;i++){

            double tValHere = 0;
            for(int j=0;j<sh;j++){
                tValHere += r.nodexs[j].coefficient * r.cFun(r.nodexs[j],r.nodexs[i].node);
            }
            double diffVec = r.nodexs[i].startValueSave - tValHere;

            r.nodexs[i].coefficientNext = r.nodexs[i].coefficient + diffVec;

            if(abs(diffVec)<abs(maxDiffVec))maxDiffVec = diffVec;
        }

        for(int i=0;i<sh;i++){
            r.nodexs[i].coefficient = r.nodexs[i].coefficientNext;
        }
    }



    

}


double RBF::cFun(const NodeExtra& n,const vec2& p2) const
{
   
    if(COEFFICIENT_CALC_TYPE==ITERATIVE_COEFFICIENT_CALC){
        double top = myKernel->KernelFun(n.node,p2);

        double bot = 0;
        for(const auto& a : nodexs){
            double a2 = myKernel->KernelFun(p2,a.node);
            bot += a2;
        }
    
       
        if(bot==0)return 0;

        return top/bot;
    }

   return myKernel->KernelFun(n.node,p2);
}

void RBF::NormalCoefficient::CalculateCoefficients(RBF& r)
{
    unsigned int sh = r.nodexs.size();
    Eigen::MatrixXf A = Eigen::MatrixXf::Random(sh, sh);
    for(int i=0;i<sh;i++){
        for(int j=0;j<sh;j++){
            A(i,j) = r.cFun(r.nodexs[i],r.nodexs[j].node);
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

double const RBF::NormalKernel::KernelFun(const vec2& p1, const vec2& p2)
{
    double preferableConstant = 50.0f;
    double distance = (p1-p2).length();

    return sqrt(distance*distance + preferableConstant * preferableConstant);
}

bool RBF::Evaluate(double _x, double _y) const
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
    double minVal = EvaluateRaw(0,0);
    double maxVal = EvaluateRaw(0,0);
    vec2 maxPlace(0,0);
    vec2 minPlace(0,0);
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            fResult[i][j] = EvaluateRaw(i,j);
            if(minVal>fResult[i][j]){minVal = fResult[i][j];minPlace = vec2(i,j);}
            if(maxVal<fResult[i][j]){maxVal = fResult[i][j];maxPlace=vec2(i,j);}
        }
    }
    //normalizing values
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            fResult[i][j] -= minVal;
            fResult[i][j] /= maxVal-minVal;
        }
    }

    std::cout<<"Szel: "<<minVal<<":"<<minPlace<<" "<<maxVal<<":"<<maxPlace<<std::endl;

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

        //Ez korrektebb lenne, csak ossze-vissza all
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
        nodexs.push_back(NodeExtra(_pos,1,nodexs.size(),_normal));
    }

}

void RBFcircle::IncreaseN()
{
    Calibrate(++currentN);
    invalidate();
    std::cout<<"Number of nodes: "<<currentN<<" ("<<nodexs.size()<<")"<<std::endl;
}

void RBFcircle::DecreaseN()
{
    if(currentN<=2)return;
    Calibrate(--currentN);
    invalidate();
    std::cout<<"Number of nodes: "<<currentN<<" ("<<nodexs.size()<<")"<<std::endl;
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
    fResult = CreateMap<double>(size,size,100);
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

RBFpolyline::RBFpolyline(size_t _size,double _h,const StrategyPack& sp) : RBF(_size,_h,sp)
{
    
}

#pragma once

#include <vector>
#include <eigen3/Eigen/Dense>
#include <cmath>
#include "general.hpp"


class RBF{
protected:

    bool valid = false;
    inline void validate(){valid=true;}
    inline void invalidate(){valid=false;}

public:
    struct NodeExtra{

        vec2 node;
        double value;


        double coefficient;

        NodeExtra(const vec2&,double _value,size_t index = -1,const vec2& _norm = vec2(0,0));

        vec2 normal;
        double coefficientNext;
        double startValueSave;

        double calcTet(const vec2& where) const;


        vec2 calcGrad(const vec2&) const;

        size_t index;
    };

public:

    std::vector<NodeExtra> nodexs;


    size_t size;

    bitmap bResult;
    floatmap fResult;

    double currentH;
    double startingH;

    vec2 CalcGradGood(const RBF::NodeExtra& n, const vec2& p) const;


    double cFun(const NodeExtra&,const vec2&) const;

    void calculateCoefficients();

    double EvaluateRaw(double _x, double _y) const;

    bool Evaluate(double _x, double _y) const;

    void AddNodeHelper(const vec2& _pos,const vec2& _normal, bool isLine = true);

    virtual void ReCalibrate() = 0;

public:

    class KernelStrategy{
    public:
        virtual double const KernelFun(const vec2&,const vec2&) = 0; 
    };
    class NormalKernel : public KernelStrategy{
    public:
        double const KernelFun(const vec2&,const vec2&) override;
    };
    class KompaktKernel : public KernelStrategy{
    public:
        double const KernelFun(const vec2&,const vec2&) override;
    };

    friend class CoefficientStrategy;

    class CoefficientStrategy{
    public:
        virtual void CalculateCoefficients(RBF&) = 0;
    };
    class NormalCoefficient : public CoefficientStrategy{
        void CalculateCoefficients(RBF&) override;
    };
    class IterativeCoefficient : public CoefficientStrategy{
        void CalculateCoefficients(RBF&) override;
    };

    struct StrategyPack{
        KernelStrategy* ks;
        CoefficientStrategy* cs;
    };

    RBF(size_t _size, double _h, const StrategyPack&);

    vec2 CalcGrad(const NodeExtra& n, const vec2& p) const;

    bitmap EvaluateAll();
    floatmap EvaluateAllRaw();


    inline bool isValid() const{return valid;}

    void IncreaseH();
    void DecreaseH();

    virtual void Reset();
private:
    KernelStrategy* myKernel;
    CoefficientStrategy* myCoefficent;
};

class RBFcircle : public RBF{

  
    vec2 circleCenter;
    double radius;

    unsigned int currentN;
    unsigned int startingN;
    
    void Calibrate(unsigned int _n);

    void ReCalibrate();

public:

    RBFcircle(size_t _size,unsigned int _n, double _h,const StrategyPack&);


    void IncreaseN();
    void DecreaseN();

    void Reset() override;

};


class RBFpolyline : public RBF{

    std::vector<vec2> cps;

    void Calibrate();

    void ReCalibrate();
public:
    RBFpolyline(size_t _size,double _h,const StrategyPack&);


    void AddNode(const vec2&);

    void Reset() override;

};
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


    struct NodeExtra{
        vec2 node;
        float value;


        float coefficient;

        NodeExtra(const vec2&,float _value);
    };



    std::vector<NodeExtra> nodexs;


    size_t size;

    bitmap bResult;
    floatmap fResult;

    float currentH;
    float startingH;


    float cFun(const vec2&,const vec2&) const;

    void calculateCoefficients();

    float EvaluateRaw(float _x, float _y) const;

    bool Evaluate(float _x, float _y) const;

    void AddNodeHelper(const vec2& _pos,const vec2& _normal, bool isLine = true);

    virtual void ReCalibrate() = 0;

public:

    class KernelStrategy{
    public:
        virtual float const KernelFun(const vec2&,const vec2&) = 0; 
    };
    class NormalKernel : public KernelStrategy{
    public:
        float const KernelFun(const vec2&,const vec2&) override;
    };
    class KompaktKernel : public KernelStrategy{
    public:
        float const KernelFun(const vec2&,const vec2&) override;
    };

    RBF(size_t _size, float _h, KernelStrategy*);



    bitmap EvaluateAll();
    floatmap EvaluateAllRaw();


    inline bool isValid() const{return valid;}

    void IncreaseH();
    void DecreaseH();

    virtual void Reset();
private:
    KernelStrategy* myKernel;
};

class RBFcircle : public RBF{

  
    vec2 circleCenter;
    float radius;

    unsigned int currentN;
    unsigned int startingN;
    
    void Calibrate(unsigned int _n);

    void ReCalibrate();

public:

    RBFcircle(size_t _size,unsigned int _n, float _h,KernelStrategy*);


    void IncreaseN();
    void DecreaseN();

    void Reset() override;

};


class RBFpolyline : public RBF{

    std::vector<vec2> cps;

    void Calibrate();

    void ReCalibrate();
public:
    RBFpolyline(size_t _size,float _h,KernelStrategy*);


    void AddNode(const vec2&);

    void Reset() override;

};
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


    float cFun(const vec2&,const vec2&);

    void calculateCoefficients();

    float EvaluateRaw(float _x, float _y);

    bool Evaluate(float _x, float _y);

    void AddNodeHelper(const vec2& _pos,const vec2& _normal, bool isLine = true);

public:
    RBF(size_t _size, float _h);



    bitmap EvaluateAll();
    floatmap EvaluateAllRaw();


    inline bool isValid(){return valid;}

};

class RBFcircle : public RBF{

  
    vec2 circleCenter;
    float radius;

    unsigned int currentN;
    
    void Calibrate(unsigned int _n);

public:

    RBFcircle(size_t _size,unsigned int _n, float _h);


    void IncreaseN();
    void DecreaseN();

};


class RBFpolyline : public RBF{

    std::vector<vec2> cps;

    void Calibrate();

public:
    RBFpolyline(size_t _size,float _h);


    void AddNode(const vec2&);




};
#pragma once

#include "NormalCreator.h"

class FunctionCreator{

    static FunctionCreator* activeEntity;

    struct Node{
        Geometry::Point3D pos;
        Geometry::Vector3D norm;

        double coefficient;
        double coefficientNext;
        double startValueSave;

        Geometry::Vector3D calcGrad(const Geometry::Point3D&) const;

        Node(const Geometry::Point3D&, const Geometry::Vector3D&);
    };

    std::vector<Node> nodes;

    void FillNodes(const NormalCreator& nc);


    double cFun(const Geometry::Point3D&, const Geometry::Point3D&) const;
    double KernelFunction(const Geometry::Point3D&, const Geometry::Point3D&) const;

    void CalculateCoefficients(unsigned int max_iteration, double max_error);

    Geometry::Vector3D CalcGrad(const Node&, const Geometry::Point3D&) const;

public:

    FunctionCreator();

    void Activate();

    static double Create(const DualContouring::Point3D&);

};


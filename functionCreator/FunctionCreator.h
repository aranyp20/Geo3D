#pragma once

#include "NormalCreator.h"

class FunctionCreator{

    static FunctionCreator* activeEntity;

    struct Node{
        Geometry::Point3D pos;
        Geometry::Vector3D norm;

        float coefficient;
        float coefficientNext;
        float startValueSave;

        Geometry::Vector3D calcGrad(const Geometry::Point3D&) const;

        Node(const Geometry::Point3D&, const Geometry::Vector3D&);
    };

    std::vector<Node> nodes;

    void FillNodes(const NormalCreator& nc);

    float KernelFunction(const Geometry::Point3D&, const Geometry::Point3D&) const;

    void CalculateCoefficients(unsigned int max_iteration, float max_error);

public:

    FunctionCreator();

    void Activate();

    static double Create(const DualContouring::Point3D&);

};


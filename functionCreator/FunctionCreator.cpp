#include <limits>
#include <math.h>
#include <stdlib.h>    
#include <time.h>  
#include "FunctionCreator.h"

FunctionCreator* FunctionCreator::activeEntity = NULL;

const double influenceZone = 200;

FunctionCreator::FunctionCreator()
{
    NormalCreator n1("inputs/sphere.obj");

    FillNodes(n1);

    CalculateCoefficients(30,0.000001f);


    Activate();
}

FunctionCreator::Node::Node(const Geometry::Point3D& _pos, const Geometry::Vector3D& _norm) : pos(_pos), norm(_norm)
{
}

double phiD(double r, double rho)
{
    return ((20*r)/(rho*rho))*(std::pow((r/rho)-1,3));
}

Geometry::Vector3D FunctionCreator::CalcGrad(const Node& n, const Geometry::Point3D& p) const
{
    double eps = 1e-7;
    return Geometry::Vector3D(((cFun(p,n.pos+Geometry::Vector3D(eps,0,0))-cFun(p,n.pos))/eps),
    ((cFun(p,n.pos+Geometry::Vector3D(0,eps,0))-cFun(p,n.pos))/eps),
    ((cFun(p,n.pos+Geometry::Vector3D(0,0,eps))-cFun(p,n.pos))/eps));
}


Geometry::Vector3D FunctionCreator::Node::calcGrad(const Geometry::Point3D& where) const
{

    double normalizedDistance = (where-pos).norm() / influenceZone;
    double distance = (where-pos).norm();
    if(normalizedDistance>1 || normalizedDistance < 0.0001)return Geometry::Vector3D(0,0,0);

   /* return Geometry::Vector3D(((where[0]-pos[0])/normalizedDistance) * (-4*powf(1-normalizedDistance,3)*(4*normalizedDistance + 1)+powf(1-normalizedDistance,4)*4),
    ((where[1]-pos[1])/normalizedDistance) * (-4*powf(1-normalizedDistance,3)*(4*normalizedDistance + 1)+powf(1-normalizedDistance,4)*4),
    ((where[2]-pos[2])/normalizedDistance) * (-4*powf(1-normalizedDistance,3)*(4*normalizedDistance + 1)+powf(1-normalizedDistance,4)*4));
    */

    return Geometry::Vector3D((((where[0]-pos[0])/normalizedDistance)*phiD(distance,influenceZone)),(((where[1]-pos[1])/normalizedDistance)*phiD(distance,influenceZone)),(((where[2]-pos[2])/normalizedDistance)*phiD(distance,influenceZone)));


}


void FunctionCreator::Activate()
{
    FunctionCreator::activeEntity = this;
}

void FunctionCreator::FillNodes(const NormalCreator& nc)
{
    srand(time(0));

    std::vector<Geometry::Point3D> verts = nc.GetVertices();
    std::vector<Geometry::Vector3D> norms = nc.GetNormals();

    std::vector<Geometry::Point3D> vertsSelected;
    std::vector<Geometry::Vector3D> normsSelected;



    for(int i=0;i<verts.size();i++)
    {
        int which = i;
        vertsSelected.push_back(verts[which]);
        normsSelected.push_back(norms[which]);
    }


    for(int i=0;i<vertsSelected.size();i++)
    {
        nodes.push_back(Node(vertsSelected[i],normsSelected[i]));
    }
}

double FunctionCreator::cFun(const Geometry::Point3D& p1, const Geometry::Point3D& p2) const
{
     double top = KernelFunction(p1,p2);

        double bot = 0;
        for(const auto& a : nodes){
           
            bot += KernelFunction(p2,a.pos);

        }
    
        if(bot==0)return 0;

        return top/bot;
}


double FunctionCreator::KernelFunction(const Geometry::Point3D& p1, const Geometry::Point3D& p2) const
{
    double normalizedDistance = (p1-p2).norm() / influenceZone;
    if(normalizedDistance<0 || normalizedDistance>1) return 0;
    return powf(1-normalizedDistance,4) * (4 * normalizedDistance + 1);
}


void FunctionCreator::CalculateCoefficients(unsigned int max_iteration, double max_error)
{
    unsigned int sh = nodes.size();


    for(int i=0;i<sh;i++){
        double startValue = 0;
        for(int j=0;j<sh;j++){
            startValue += (nodes[i].norm*CalcGrad(nodes[j],nodes[i].pos));  
        }
        startValue *=-1;
        nodes[i].startValueSave = startValue;
        nodes[i].coefficient = startValue;
    }



    double maxDiffVec = std::numeric_limits<double>::max();
    for(int k = 0;k<max_iteration && fabs(maxDiffVec)>max_error;k++){
        for(int i=0;i<sh;i++){

            double tValHere = 0;
            for(int j=0;j<sh;j++){
                tValHere += nodes[j].coefficient * cFun(nodes[i].pos,nodes[j].pos);
            }
            double diffVec = nodes[i].startValueSave - tValHere;

            if(fabs(diffVec)<fabs(maxDiffVec))maxDiffVec = diffVec;

            nodes[i].coefficientNext = nodes[i].coefficient + diffVec;
        }

        for(int i=0;i<sh;i++){
            nodes[i].coefficient = nodes[i].coefficientNext;
        }
    }


}


int main()
{
    FunctionCreator f1;

    std::array<DualContouring::Point3D, 2> bounding_box{{{-100,-100,-100},{100,100,100}}};
    std::array<size_t, 3> resolution{10,10,10};

    isosurface(FunctionCreator::Create,0,bounding_box,resolution).writeOBJ("finalOBJ.obj");

    return EXIT_SUCCESS;
}


Geometry::Point3D PointConversion(const DualContouring::Point3D& p)
{
    return Geometry::Point3D(p.data[0],p.data[1],p.data[2]);
}

int iter = 0;

double FunctionCreator::Create(const DualContouring::Point3D& p_dc)
{
    Geometry::Point3D p = PointConversion(p_dc);


    double left = 0;
    double right = 0;

    for(int i=0;i<activeEntity->nodes.size();i++){
                

        left += activeEntity->nodes[i].coefficient * activeEntity->cFun(activeEntity->nodes[i].pos,p);
        right +=(activeEntity->nodes[i].norm*activeEntity->CalcGrad(activeEntity->nodes[i],p));
    }

    

    return (double)(left + right);    
}
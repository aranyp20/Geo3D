#include <limits>
#include <math.h>
#include <stdlib.h>    
#include <time.h>  
#include "FunctionCreator.h"

FunctionCreator* FunctionCreator::activeEntity = NULL;

const float influenceZone = 20;

FunctionCreator::FunctionCreator()
{
    NormalCreator n1("inputs/stanford-bunny.obj");

    FillNodes(n1);

    CalculateCoefficients(3,0.01f);

    Activate();
}

FunctionCreator::Node::Node(const Geometry::Point3D& _pos, const Geometry::Vector3D& _norm) : pos(_pos), norm(_norm)
{
}

Geometry::Vector3D FunctionCreator::Node::calcGrad(const Geometry::Point3D& where) const
{

    float normalizedDistance = (where-pos).norm() / influenceZone;
    if(normalizedDistance>1 || normalizedDistance < 0.0001)return Geometry::Vector3D(0,0,0);

    return Geometry::Vector3D(((where[0]-pos[0])/normalizedDistance) * (-4*powf(1-normalizedDistance,3)*(4*normalizedDistance + 1)+powf(1-normalizedDistance,4)*4),
    ((where[1]-pos[1])/normalizedDistance) * (-4*powf(1-normalizedDistance,3)*(4*normalizedDistance + 1)+powf(1-normalizedDistance,4)*4),
    ((where[2]-pos[2])/normalizedDistance) * (-4*powf(1-normalizedDistance,3)*(4*normalizedDistance + 1)+powf(1-normalizedDistance,4)*4));
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



    for(int i=0;i<100;i++)
    {
        int which = rand()%100;
        vertsSelected.push_back(verts[which]);
        normsSelected.push_back(norms[which]);
    }


    for(int i=0;i<vertsSelected.size();i++)
    {
        nodes.push_back(Node(vertsSelected[i],normsSelected[i]));
    }
}

float FunctionCreator::KernelFunction(const Geometry::Point3D& p1, const Geometry::Point3D& p2) const
{
    float normalizedDistance = (p1-p2).norm() / influenceZone;
    if(normalizedDistance<0 || normalizedDistance>1) return 0;
    return powf(1-normalizedDistance,4) * (4 * normalizedDistance + 1);
}


void FunctionCreator::CalculateCoefficients(unsigned int max_iteration, float max_error)
{
    unsigned int sh = nodes.size();


    for(int i=0;i<sh;i++){
        float startValue = 0;
        for(int j=0;j<sh;j++){
            startValue += (nodes[i].norm*nodes[j].calcGrad(nodes[i].pos));  
        }
        startValue *=-1;
        nodes[i].startValueSave = startValue;
        nodes[i].coefficient = startValue;
    }



    float maxDiffVec = std::numeric_limits<float>::max();
    for(int k = 0;k<max_iteration && fabs(maxDiffVec)>max_error;k++){
        for(int i=0;i<sh;i++){

            float tValHere = 0;
            for(int j=0;j<sh;j++){
                tValHere += nodes[j].coefficient * KernelFunction(nodes[i].pos,nodes[j].pos);
            }
            float diffVec = nodes[i].startValueSave - tValHere;

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

    std::array<DualContouring::Point3D, 2> bounding_box{(-1,-1,-1),(1,1,1)};
    std::array<size_t, 3> resolution{100,100,100};

    isosurface(FunctionCreator::Create,3,bounding_box,resolution).writeOBJ("finalOBJ.obj");

    return EXIT_SUCCESS;
}


Geometry::Point3D PointConversion(const DualContouring::Point3D& p)
{
    return Geometry::Point3D(p.data[0],p.data[1],p.data[2]);
}

double FunctionCreator::Create(const DualContouring::Point3D& p_dc)
{
    Geometry::Point3D p = PointConversion(p_dc);

    float left = 0;
    float right = 0;

    for(int i=0;i<activeEntity->nodes.size();i++){
        left += activeEntity->nodes[i].coefficient * activeEntity->KernelFunction(activeEntity->nodes[i].pos,p);
        right +=(activeEntity->nodes[i].norm*activeEntity->nodes[i].calcGrad(p));
    }

    

    return (double)(left + right);    
}
#pragma once

#include <vector>
#include <map>
#include "dependencies/dualcontouring/dual-contouring/dc.hh"
#include "dependencies/libgeom/geometry.hh"

typedef std::vector<std::vector<unsigned int>> imap;


class NormalCreator{

    struct InputState{
    enum state{VERT,FACE,NONE};
        state myState;
        inline void Init(state _state){myState = _state;remaining=3;}
        inline state Step(){if(remaining==0){myState = NONE;}else{ remaining--;} return myState;}
    private:
        int remaining = 3;

    };


    std::map<unsigned int, std::vector<unsigned int>> influencedBy;
    std::vector<Geometry::Point3D> vertices;
    imap faces;
    std::vector<Geometry::Vector3D> normals;

    imap FindFacesOfVert(unsigned int);
    std::vector<Geometry::Vector3D> CreateFaceNormals(const imap&);

    void CreateNormal(unsigned int);
    void CreateNormals();


public:

    NormalCreator(const std::string&);

    inline std::vector<Geometry::Vector3D> GetNormals() const {return normals;}
    inline std::vector<Geometry::Point3D> GetVertices() const {return vertices;}


};
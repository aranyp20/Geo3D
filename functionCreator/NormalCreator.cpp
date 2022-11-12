#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include "NormalCreator.h"


NormalCreator::NormalCreator(const std::string& filename)
{
    std::string outputfile("outputs/result.vtk");
    std::ofstream writer(outputfile);

    std::fstream file;
    std::string word;
  


    file.open(filename.c_str());
  
    InputState is;
    
    std::vector<double> cVert;
    std::vector<unsigned int> cFace;


    while (file >> word)
    {
        

        if(word.compare("v")==0){
            is.Init(InputState::VERT);
        }else if(word.compare("f")==0){
            is.Init(InputState::FACE);
        }else {
            InputState::state stateNow = is.Step();  
            if(stateNow==InputState::VERT){
                cVert.push_back(stod(word));
            }else if(stateNow==InputState::FACE){
                cFace.push_back(stoi(word));
            }

        }


        if(cVert.size()==3){
            vertices.push_back(Geometry::Vector3D(cVert[0],cVert[1],cVert[2]));
            std::vector<unsigned int> tv;
            influencedBy.insert({vertices.size()-1,tv});
            cVert.clear();
        }
        if(cFace.size()==3){
            faces.push_back(cFace);
            influencedBy[cFace[0]].push_back(faces.size()-1);
            influencedBy[cFace[1]].push_back(faces.size()-1);
            influencedBy[cFace[2]].push_back(faces.size()-1);
            cFace.clear();
        }

    }


    CreateNormals();

    
    writer<<"# vtk DataFile Version 2.0"<<std::endl;
    writer<<"Alma"<<std::endl;
    writer<<"ASCII"<<std::endl;
    writer<<"DATASET POLYDATA"<<std::endl;
    writer<<"POINTS "<<vertices.size()<<" float"<<std::endl;
    for(const auto& a : vertices){
        writer<<a<<std::endl;
    }
    writer<<"POINT_DATA "<<vertices.size()<<std::endl;
    writer<<"NORMALS normal float"<<std::endl;
    for(const auto& a : normals){
        writer<<a<<std::endl;
    }
}



imap NormalCreator::FindFacesOfVert(unsigned int pi)
{
    imap result;
    for(const auto& a : influencedBy[pi]){
       result.push_back(faces[a]);
    }

    return result;
}

std::vector<Geometry::Vector3D> NormalCreator::CreateFaceNormals(const imap& im)
{
    std::vector<Geometry::Vector3D> result;

    for(const auto& a : im){
        result.push_back(((vertices[a[1]-1]-vertices[a[0]-1])^(vertices[a[2]-1]-vertices[a[0]-1])).normalize());
    }

    return result;
}

void NormalCreator::CreateNormal(unsigned int pi)
{
    std::vector<Geometry::Vector3D> fNormals = CreateFaceNormals(FindFacesOfVert(pi+1));
    Geometry::Vector3D result(0,0,0);
    for(const auto& a : fNormals){
        result = result + a; 
    }
    result = result/fNormals.size(); 
    if(fNormals.size()==0){
        normals.push_back(Geometry::Point3D(0,0,0));
    }else{

        normals.push_back(result);
    }
}

void NormalCreator::CreateNormals()
{

    for(unsigned int i = 0;i<vertices.size();i++){
        CreateNormal(i);
    }

   
}


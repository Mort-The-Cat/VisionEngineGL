#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

class vec3
{
public:
    float x, y, z;
    bool operator==(const vec3& Other)
    {
        return x == Other.x && y == Other.y && z == Other.z;
    }
};

class Model_Vertex
{
public:
    vec3 Position, Normal;      // For vertex animations, this is all we need. We can refactor this later if we want to add more features

    bool operator==(const Model_Vertex& Other)
    {
        return Position == Other.Position && Normal == Other.Normal;
    }
};

bool Already_In_Vector(std::vector<Model_Vertex>* Vector, Model_Vertex Vertex)
{
    for (size_t W = 0; W < Vector->size(); W++)
        if (Vector->at(W) == Vertex)
            return true;
    return false;
}

int main()
{
    std::string Keyframe_Name;
    std::string Animation_Duration, Tickrate;

    printf("Please enter the keyframe name:\n");
    std::cin >> Keyframe_Name;
    printf("Please enter the animation length (ticks):\n");
    std::cin >> Animation_Duration;
    printf("Please enter the framerate (fps):\n");
    std::cin >> Tickrate;

    // Once we've got all the user inputs...

    std::ofstream File_Out(Keyframe_Name + ".anim", std::ios::out);
    File_Out.write((Tickrate + "\n").c_str(), Tickrate.length() + 1);
    File_Out.write((Animation_Duration + "\n").c_str(), Animation_Duration.length() + 1);

    size_t Duration = std::atoi(Animation_Duration.c_str());

    for (size_t I = 1; I <= Duration; I++)
    {
        std::string Buffer = std::to_string(I) + "\n";
        File_Out.write(Buffer.c_str(), Buffer.length());
    }

    for (size_t I = 1; I <= Duration; I++)
    {
        File_Out.write("\n", 1); // I think this is right?

#define Obj tinyobj
        Obj::attrib_t Attributes;
        std::vector<Obj::shape_t> Shapes;
        std::vector<Obj::material_t> Materials;

        std::string Warning, Error;

        if (!Obj::LoadObj(&Attributes, &Shapes, &Materials, &Warning, &Error, ("Assets/" + Keyframe_Name + std::to_string(I) + ".obj").c_str()))
        {
            throw std::invalid_argument("Unable to load keyframe file!\n");
        }

        std::vector<Model_Vertex> Vertices;

        for (const auto& Shape : Shapes)
        {
            for (const auto& Index : Shape.mesh.indices)
            {
                Model_Vertex Vertex;

                Vertex.Position = {
                    Attributes.vertices[3 * Index.vertex_index],
                    -Attributes.vertices[3 * Index.vertex_index + 1],
                    Attributes.vertices[3 * Index.vertex_index + 2]
                };

                Vertex.Normal = {
                    Attributes.normals[3 * Index.normal_index],
                    -Attributes.normals[3 * Index.normal_index + 1],
                    Attributes.normals[3 * Index.normal_index + 2]
                };

                //Vertex.Occlusion = 1;

                //Vertices.push_back(Vertex);
                //Indices.push_back(Indices.size()); // We'll adjust this later

                if (!Already_In_Vector(&Vertices, Vertex))
                    Vertices.push_back(Vertex);
            }
        }

        //

        for (size_t W = 0; W < Vertices.size(); W++)
        {
            std::string Buffer =
                std::to_string(Vertices[W].Position.x) + " " + 
                std::to_string(Vertices[W].Position.y) + " " + 
                std::to_string(Vertices[W].Position.z) + " " + 
                std::to_string(Vertices[W].Normal.x) + " " + 
                std::to_string(Vertices[W].Normal.y) + " " +
                std::to_string(Vertices[W].Normal.z) + "\n";

            File_Out.write(Buffer.c_str(), Buffer.length());
        }
    }

    File_Out.close();

    return 0;
}
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <sstream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Float_Text_Encoder.h"

class vec3
{
public:
    float x, y, z;
    bool operator==(const vec3& Other)
    {
        return x == Other.x && y == Other.y && z == Other.z;
    }
};

class vec2
{
public:
    float x, y;
    bool operator==(const vec2& Other)
    {
        return x == Other.x && Other.y == y;
    }
};

class Model_Vertex
{
public:
    vec3 Position, Normal;      // For vertex animations, this is all we need. We can refactor this later if we want to add more features

    vec2 UVs;

    bool operator==(const Model_Vertex& Other)
    {
        return Position == Other.Position && Normal == Other.Normal && UVs == Other.UVs;
    }
};

bool Already_In_Vector(std::vector<Model_Vertex>* Vector, Model_Vertex Vertex)
{
    for (size_t W = 0; W < Vector->size(); W++)
        if (Vector->at(W) == Vertex)
            return true;
    return false;
}

void Pad_Left_String(std::string* String, unsigned char Desired_Length)
{
    while (String->length() < Desired_Length)
        *String = "0" + *String;

    *String = "_" + *String;
}

struct Vertex_Indices
{
    size_t Positions_Index, Normals_Index, UV_Index;

    bool operator==(const Vertex_Indices Other)
    {
        return Positions_Index == Other.Positions_Index && Normals_Index == Other.Normals_Index && UV_Index == Other.UV_Index;
    }
};

bool Identify_Existing_Vertex(std::vector<Vertex_Indices>& Unique_Vertices, Vertex_Indices Vertex, size_t* Index)
{
    for (size_t W = 0; W < Unique_Vertices.size(); W++)
        if (Unique_Vertices[W] == Vertex)
        {
            *Index = W;
            return true;
        }
    return false;
}

std::vector<Model_Vertex> Load_Model(std::string File_Name)
{
    std::ifstream File(File_Name);

    std::string Line;

    std::vector<Model_Vertex> Vertices;
    std::vector<vec3> Positions;
    std::vector<vec3> Normals;
    std::vector<vec2> UVs;

    std::vector<Vertex_Indices> Unique_Vertices;

    if (File.is_open())
    {
        while (std::getline(File, Line))
        {
            if (Line[0] != '#')
            {
                // If this line isn't a comment,

                std::stringstream Stream;
                Stream << Line;

                std::string Prefix;
                Stream >> Prefix;

                if (Prefix == "v")
                {
                    Positions.push_back(vec3());
                    Stream >> Positions.back().x >> Positions.back().y >> Positions.back().z;
                    Positions.back().y *= -1;
                }
                else if (Prefix == "vn")
                {
                    Normals.push_back(vec3());
                    Stream >> Normals.back().x >> Normals.back().y >> Normals.back().z;
                    Normals.back().y *= -1;
                }
                else if (Prefix == "vt")
                {
                    UVs.push_back(vec2());
                    Stream >> UVs.back().x >> UVs.back().y;
                }
                else if (Prefix == "f")
                {
                    Stream = std::stringstream();
                    for (size_t W = 0; W < Line.size(); W++)
                        if (Line[W] == '/')
                            Line[W] = ' ';

                    Stream << Line;
                    Stream >> Prefix;

                    for (size_t W = 0; W < 3; W++)
                    {
                        size_t Positions_Index, Normals_Index, UV_Index;

                        Stream >> Positions_Index >> UV_Index >> Normals_Index;

                        Model_Vertex Vertex;
                        Vertex.Position = Positions[Positions_Index - 1];
                        Vertex.Normal = Normals[Normals_Index - 1];
                        Vertex.UVs = UVs[UV_Index - 1];

                        //if (!Identify_Existing_Vertex(Unique_Vertices, Indices, &Index))
                        //{
                        Vertices.push_back(Vertex);
                        //    Unique_Vertices.push_back(Indices);
                        //}

                        //if (!Already_In_Vector(&Vertices, Vertex))
                        //{
                        //    Vertices.push_back(Vertex);
                        //}
                    }
                }
            }
        }
    }

    return Vertices;
}

int main()
{
    std::string Keyframe_Name;
    std::string Animation_Duration, Tickrate, Anim_Start;

    std::string Step;

    printf("Please enter the keyframe name:\n");
    std::cin >> Keyframe_Name;
    printf("Please enter the animation length (ticks):\n");
    std::cin >> Animation_Duration;
    printf("Please enter the framerate (fps):\n");
    std::cin >> Tickrate;
    printf("Please enter the animation start:\n");
    std::cin >> Anim_Start;
    printf("Finally, enter the step of the animation:\n");
    std::cin >> Step;

    // Once we've got all the user inputs...

    std::ofstream File_Out(Keyframe_Name + ".anim", std::ios::out);
    File_Out.write((Tickrate + "\n").c_str(), Tickrate.length() + 1);
    File_Out.write((Step + "\n").c_str(), Step.length() + 1);
    File_Out.write((Animation_Duration + "\n").c_str(), Animation_Duration.length() + 1);

    size_t Duration = std::atoi(Animation_Duration.c_str());

    size_t Step_Value = std::atoi(Step.c_str());

    size_t Animation_Start = std::atoi(Anim_Start.c_str());

    for (size_t I = Animation_Start; I < Duration + Animation_Start; I += Step_Value)
    {
        File_Out.write("\n", 1); // I think this is right?

        std::string Number_Suffix = std::to_string(I);
        //Pad_Left_String(&Number_Suffix, 6);

        std::vector<Model_Vertex> Vertices = Load_Model("Assets/" + Keyframe_Name + Number_Suffix + ".obj");

/*#define Obj tinyobj
        Obj::attrib_t Attributes;
        std::vector<Obj::shape_t> Shapes;
        std::vector<Obj::material_t> Materials;

        std::string Warning, Error;

        std::string Number_Suffix = std::to_string(I);
        Pad_Left_String(&Number_Suffix, 6);

        if (!Obj::LoadObj(&Attributes, &Shapes, &Materials, &Warning, &Error, ("Assets/" + Keyframe_Name + Number_Suffix + ".obj").c_str()))
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

        */

        //

        for (size_t W = 0; W < Vertices.size(); W++)
        {
            //std::string Float_Representation = Encoder::Float_To_Characters(Vertices[W].Position.x);

            //float Number = Encoder::Characters_To_Float(Float_Representation.c_str());

            /*std::string Buffer =
                std::to_string(Vertices[W].Position.x) + " " + 
                std::to_string(Vertices[W].Position.y) + " " + 
                std::to_string(Vertices[W].Position.z) + " " + 
                std::to_string(Vertices[W].Normal.x) + " " + 
                std::to_string(Vertices[W].Normal.y) + " " +
                std::to_string(Vertices[W].Normal.z) + "\n";*/

            std::string Buffer =
                Encoder::Float_To_Characters(Vertices[W].Position.x) +
                Encoder::Float_To_Characters(Vertices[W].Position.y) +
                Encoder::Float_To_Characters(Vertices[W].Position.z) +
                Encoder::Float_To_Characters(Vertices[W].Normal.x) +
                Encoder::Float_To_Characters(Vertices[W].Normal.y) +
                Encoder::Float_To_Characters(Vertices[W].Normal.z) + "\n";

            Model_Vertex Read_Vertex;

            Read_Vertex.Position.x = Encoder::Characters_To_Float(&Buffer.c_str()[0]);
            Read_Vertex.Position.y = Encoder::Characters_To_Float(&Buffer.c_str()[6]);
            Read_Vertex.Position.z = Encoder::Characters_To_Float(&Buffer.c_str()[12]);

            Read_Vertex.Normal.x = Encoder::Characters_To_Float(&Buffer.c_str()[18]);
            Read_Vertex.Normal.y = Encoder::Characters_To_Float(&Buffer.c_str()[24]);
            Read_Vertex.Normal.z = Encoder::Characters_To_Float(&Buffer.c_str()[30]);

            if (!(Read_Vertex.Position == Vertices[W].Position) || !(Read_Vertex.Normal == Vertices[W].Normal))
                printf("Serious error!");

            File_Out.write(Buffer.c_str(), Buffer.length());
        }
    }

    File_Out.close();

    return 0;
}
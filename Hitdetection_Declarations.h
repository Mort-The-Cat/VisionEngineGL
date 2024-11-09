#ifndef HITDETECTION_DECLARATIONS_VISIONGL
#define HITDETECTION_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Model_Declarations.h"

class Collision_Info
{
public:
	glm::vec3 Collision_Position = { 0, 0, 0 };
	glm::vec3 Collision_Normal = { 0, 0, 0 };
	float Overlap = 0;

	Collision_Info() {}
	Collision_Info(glm::vec3 Collision_Positionp, glm::vec3 Collision_Normalp, float Overlapp)
	{
		Collision_Position = Collision_Positionp;
		Collision_Normal = Collision_Normalp;
		Overlap = Overlapp;
	}
};

Collision_Info Inverse_Collision(Collision_Info Info)
{
	Info.Collision_Normal *= -1;

	return Info;
}

class AABB_Hitbox;
class Sphere_Hitbox;
class Mesh_Hitbox;

#define HF_TO_BE_DELETED 0u

class Hitbox
{
public:
	glm::vec3* Position;
	Model* Object;

	bool Flags[1] = { false };

	Hitbox() {}
	Hitbox(glm::vec3* Positionp) { Position = Positionp; }

	virtual Collision_Info Hitdetection(Hitbox* Other) { return Collision_Info(); }

	virtual Collision_Info AABB_Hitdetection(AABB_Hitbox* Other) { return Collision_Info(); }
	virtual Collision_Info Sphere_Hitdetection(Sphere_Hitbox* Other) { return Collision_Info(); }
	virtual Collision_Info Mesh_Hitdetection(Mesh_Hitbox* Other) { return Collision_Info(); }

	virtual void Update_Hitbox() {  }
};

class AABB_Hitbox : public Hitbox
{
public:
	glm::vec3 A = glm::vec3(999,999,999), B = glm::vec3(-999,-999,-999);
	virtual Collision_Info Hitdetection(Hitbox* Other) override
	{
		return Other->AABB_Hitdetection(this);
	}

	virtual Collision_Info AABB_Hitdetection(AABB_Hitbox* Other) override;
	virtual Collision_Info Sphere_Hitdetection(Sphere_Hitbox* Other) override;
	virtual Collision_Info Mesh_Hitdetection(Mesh_Hitbox* Other) override;
};

class Sphere_Hitbox : public Hitbox
{
public:
	float Radius;

	virtual Collision_Info Hitdetection(Hitbox* Other) override
	{ 
		return Other->Sphere_Hitdetection(this); 
	}

	virtual Collision_Info AABB_Hitdetection(AABB_Hitbox* Other) override;
	virtual Collision_Info Sphere_Hitdetection(Sphere_Hitbox* Other) override;
	virtual Collision_Info Mesh_Hitdetection(Mesh_Hitbox* Other) override;
};

glm::mat4 Hitbox_Direction_Matrix_Calculate(Model* Object);

class Mesh_Hitbox : public Hitbox
{
public:
	std::vector<glm::vec3> Vertices;
	std::vector<uint32_t> Indices; // These are the indices for each tri !

	std::vector<glm::vec3> Transformed_Vertices; // These are the vertices that have been transformed with the rotation matrices and the object's hitbox's position

	glm::mat3 Matrix_Rotation = glm::mat3(1.0f); // This is the matrix rotation applied to the vertices during hit detection- perhaps I can initialise this from the control function

	Mesh_Hitbox() {}

	virtual void Update_Hitbox() override
	{
		Matrix_Rotation = Hitbox_Direction_Matrix_Calculate(Object);

		for (size_t W = 0; W < Vertices.size(); W++)
			Transformed_Vertices[W] = Matrix_Rotation * Vertices[W];
	}

	virtual Collision_Info Hitdetection(Hitbox* Other) override
	{
		return Other->Mesh_Hitdetection(this);
	}

	virtual Collision_Info Mesh_Hitdetection(Mesh_Hitbox* Other) override;
	virtual Collision_Info Sphere_Hitdetection(Sphere_Hitbox* Other) override;
	virtual Collision_Info AABB_Hitdetection(AABB_Hitbox* Other) override;

	bool Normal_Already_Included(glm::vec3 Normal)
	{
		for (size_t W = 0; W < Indices.size(); W += 3)
		{
			glm::vec3 Calculated_Normal = Calculate_Surface_Normal(Vertices[Indices[W]], Vertices[Indices[W + 1]], Vertices[Indices[W + 2]]);

			if (Normal == Calculated_Normal)
				return true;
		}

		return false;
	}
};

std::vector<Hitbox*> Scene_Hitboxes; // The first n objects (where n = Scene_Physics_Objects.size()) are always physics objects

std::vector<Hitbox*> Generate_AABB_Hitboxes(std::string File_Name)
{
	std::vector<Hitbox*> Return;

	// This will load the file of the hitbox

	std::ifstream File(File_Name);

	if (File.is_open())
	{
		std::string Line;

		size_t Count = 0u;

		while (std::getline(File, Line))
		{
			std::stringstream Buffer;

			Buffer << Line;

			std::string Prefix;

			Buffer >> Prefix;

			if (Prefix == "o" || Prefix == "g" ||
				(Count == 8)) // This signals that a new hitbox object needs to be created
			{
				Return.push_back(new AABB_Hitbox());

				Count = 0;
			}

			if (Prefix == "v") // This will tell the engine to evaluate and add the positions of this object's vertices to the hitbox
			{
				glm::vec3 Vertex;

				Buffer >> Vertex.x >> Vertex.y >> Vertex.z;

				Vertex.y *= -1;

				//

				((AABB_Hitbox*)Return.back())->A.x = std::fminf(reinterpret_cast<AABB_Hitbox*>(Return.back())->A.x, Vertex.x);
				((AABB_Hitbox*)Return.back())->A.y = std::fminf(reinterpret_cast<AABB_Hitbox*>(Return.back())->A.y, Vertex.y);
				((AABB_Hitbox*)Return.back())->A.z = std::fminf(reinterpret_cast<AABB_Hitbox*>(Return.back())->A.z, Vertex.z);

				((AABB_Hitbox*)Return.back())->B.x = std::fmaxf(reinterpret_cast<AABB_Hitbox*>(Return.back())->B.x, Vertex.x);
				((AABB_Hitbox*)Return.back())->B.y = std::fmaxf(reinterpret_cast<AABB_Hitbox*>(Return.back())->B.y, Vertex.y);
				((AABB_Hitbox*)Return.back())->B.z = std::fmaxf(reinterpret_cast<AABB_Hitbox*>(Return.back())->B.z, Vertex.z);

				Count++;
			}
		}

		File.close();

		return Return;
	}
	else
		Throw_Error("Unable to open hitbox file!\n");
}

/*std::vector<Hitbox*> Generate_AABB_Hitboxes(Model_Mesh& Mesh)
{
	std::vector<Hitbox*> Return;

	for (size_t Cube = 0; Cube < Mesh.Vertices.size(); Cube += 8) // Cubes have 8 vertices xd I'm silly
	{
		Return.push_back(new AABB_Hitbox());

		for (size_t W = Cube; W < Cube + 8; W++)
		{
			((AABB_Hitbox*)Return.back())->A.x = std::fminf(((AABB_Hitbox*)Return.back())->A.x, Mesh.Vertices[W].Position.x);
			((AABB_Hitbox*)Return.back())->A.y = std::fminf(((AABB_Hitbox*)Return.back())->A.y, Mesh.Vertices[W].Position.y);
			((AABB_Hitbox*)Return.back())->A.z = std::fminf(((AABB_Hitbox*)Return.back())->A.z, Mesh.Vertices[W].Position.z);

			((AABB_Hitbox*)Return.back())->B.x = std::fmaxf(((AABB_Hitbox*)Return.back())->B.x, Mesh.Vertices[W].Position.x);
			((AABB_Hitbox*)Return.back())->B.y = std::fmaxf(((AABB_Hitbox*)Return.back())->B.y, Mesh.Vertices[W].Position.y);
			((AABB_Hitbox*)Return.back())->B.z = std::fmaxf(((AABB_Hitbox*)Return.back())->B.z, Mesh.Vertices[W].Position.z);
		}
	}

	return Return;
}*/

AABB_Hitbox* Generate_AABB_Hitbox(Model_Mesh& Mesh)
{
	AABB_Hitbox* Return = new AABB_Hitbox();
	for (size_t W = 0; W < Mesh.Vertices.size(); W++)
	{
		Return->A.x = std::fminf(Return->A.x, Mesh.Vertices[W].Position.x);
		Return->A.y = std::fminf(Return->A.y, Mesh.Vertices[W].Position.y);
		Return->A.z = std::fminf(Return->A.z, Mesh.Vertices[W].Position.z);

		Return->B.x = std::fmaxf(Return->B.x, Mesh.Vertices[W].Position.x);
		Return->B.y = std::fmaxf(Return->B.y, Mesh.Vertices[W].Position.y);
		Return->B.z = std::fmaxf(Return->B.z, Mesh.Vertices[W].Position.z);
	}

	return Return;
}

Sphere_Hitbox* Generate_Sphere_Hitbox(Model_Mesh& Mesh)
{
	Sphere_Hitbox* Return = new Sphere_Hitbox();
	for (size_t W = 0; W < Mesh.Vertices.size(); W++)
	{
		Return->Radius = std::fmaxf(Return->Radius, glm::dot(Mesh.Vertices[W].Position, Mesh.Vertices[W].Position));
	}

	Return->Radius = sqrtf(Return->Radius);

	return Return;
}

Mesh_Hitbox* Generate_Mesh_Hitbox(Model_Mesh& Mesh)
{
	Mesh_Hitbox* Hitbox = new Mesh_Hitbox();

	// Get indices for all of the edges in the mesh

	// For this, it'll be good to calculate all of the 

	Hitbox->Vertices.resize(Mesh.Vertices.size());

	//Hitbox->Indices = Mesh.Indices; // We can copy this 1-1

	for (size_t W = 0; W < Mesh.Vertices.size(); W++)
		Hitbox->Vertices[W] = Mesh.Vertices[W].Position;

	for (size_t W = 0; W < Mesh.Indices.size(); W += 3) // We're dealing with triangles so that's how we'll iterate
	{
		glm::vec3 Normal = Calculate_Surface_Normal(Mesh.Vertices[Mesh.Indices[W]].Position, Mesh.Vertices[Mesh.Indices[W + 1]].Position, Mesh.Vertices[Mesh.Indices[W + 2]].Position);

		if (!Hitbox->Normal_Already_Included(Normal))
		{
			Hitbox->Indices.reserve(Hitbox->Indices.size() + 3);

			for (char V = 0; V < 3; V++)
				Hitbox->Indices.push_back(Mesh.Indices[W + V]);
		}

		// We check if we've included this normal in the model before!

	}

	// I believe that makes it a success!!

	/*printf("\n\n");

	for (size_t W = 0; W < Hitbox->Vertices.size(); W++)
		printf("glm::vec3(%sX, %sY, %sZ),\n",
			Hitbox->Vertices[W].x > 0 ? "Max" : "Min",
			Hitbox->Vertices[W].y > 0 ? "Max" : "Min",
			Hitbox->Vertices[W].z > 0 ? "Max" : "Min");*/

	Hitbox->Matrix_Rotation = glm::mat3(1.0f);

	Hitbox->Transformed_Vertices = Hitbox->Vertices;

	return Hitbox;
}

#endif
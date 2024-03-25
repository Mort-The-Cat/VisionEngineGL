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
};

class AABB_Hitbox : public Hitbox
{
public:
	glm::vec3 A, B;
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

class Mesh_Hitbox : public Hitbox
{
public:
	std::vector<glm::vec3> Vertices;
	std::vector<uint32_t> Indices; // These are the indices for each tri !

	std::vector<glm::vec3> Transformed_Vertices; // These are the vertices that have been transformed with the rotation matrices and the object's hitbox's position

	glm::mat3 Matrix_Rotation; // This is the matrix rotation applied to the vertices during hit detection- perhaps I can initialise this from the control function

	Mesh_Hitbox() {}

	void Update_Vertices()
	{
		for (size_t W = 0; W < Vertices.size(); W++)
			Transformed_Vertices[W] = Matrix_Rotation * Vertices[W];

		// This should be okay
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

	Hitbox->Matrix_Rotation = glm::mat3(1);

	Hitbox->Transformed_Vertices = Hitbox->Vertices;

	return Hitbox;
}

#endif
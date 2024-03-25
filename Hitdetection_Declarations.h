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

#endif

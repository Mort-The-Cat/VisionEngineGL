#ifndef PARTICLE_SYSTEM_DECLARATIONS_VISIONGL
#define PARTICLE_SYSTEM_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Texture_Declarations.h"

class Particle_Info
{
public:
	size_t Count;
	size_t Particles_Per_Call;

	virtual void Parse_Values(size_t Indices)
	{

	}

	virtual void Update()
	{

	}
};

class Particle_Renderer
{
public:
	Shader Shader;
	Particle_Info* Particles;
	Model_Vertex_Buffer Mesh;
	Texture Albedo;
	Texture Material;
	void Update()
	{
		Particles->Update();	// This will update all of the particles to account for the new delta time
	}
	void Render()
	{
		Shader.Activate();
		Mesh.Bind_Buffer();

		Albedo.Parse_Texture(Shader, "Albedo", 0);
		Albedo.Bind_Texture();

		Material.Parse_Texture(Shader, "Material", 1);
		Material.Bind_Texture();

		for (size_t W = Particles->Count; W < Particles->Count; W += Particles->Particles_Per_Call)
		{
			Particles->Parse_Values(W);

			glDrawElementsInstanced(GL_TRIANGLES, Mesh.Indices_Count, GL_UNSIGNED_INT, 0, Particles->Particles_Per_Call);
		}
	}
};



#endif
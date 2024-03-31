#ifndef PARTICLE_SYSTEM_DECLARATIONS_VISIONGL
#define PARTICLE_SYSTEM_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Texture_Declarations.h"

struct Smoke_Particle
{
	glm::vec4 Position; // .w is age
	glm::vec4 Velocity; // .w is transparency
};

bool Smoke_Particle_Remove_If(const Smoke_Particle& A)
{
	return A.Position.w > 4; // Smoke disappears after 10 seconds
}

template<typename Particle>
class Particle_Info
{
public:
	size_t Count;
	size_t Particles_Per_Call;
	std::vector<Particle> Particles_Data;

	Camera_Uniform_Location_Object Camera_Location;
	Light_Uniform_Location_Object Light_Location;

	int Uniform_Location; // We're parsing this all to one array in the GPU
	size_t Float_Size;

	void Init_Shader_Information(Shader Shader)
	{
		Shader.Activate();
		Uniform_Location = glGetUniformLocation(Shader.Program_ID, "Particle_Data");

		Camera_Location = Initialise_Camera_Uniform_Locations_Object(Shader);
		Light_Location = Initialise_Light_Uniform_Locations_Object(Shader);

		Float_Size = sizeof(Particle) >> 2;
	}

	size_t Parse_Values(size_t Index) 
	{
		size_t Count = std::min(Particles_Per_Call, Particles_Data.size() - Index);
		glUniform1fv(Uniform_Location, Count * Float_Size, (const GLfloat*)(&Particles_Data[Index]));

		return Count;
	}

};

class Smoke_Particle_Info : public Particle_Info<Smoke_Particle>
{
public:
	// This has 8 floating point values

	Smoke_Particle_Info()
	{
		Particles_Per_Call = 200;
	}

	void Spawn_Particle(glm::vec3 Position, glm::vec3 Velocity)
	{
		Smoke_Particle New_Particle;
		New_Particle.Position = glm::vec4(Position, 0);
		New_Particle.Velocity = glm::vec4(Velocity, RNG() * 24 - 12);
		Particles_Data.push_back(New_Particle);
	}

	void Update()
	{
		for (size_t W = 0; W < Particles_Data.size(); W++)
			Particles_Data[W].Position.w += Tick;

		auto Particles_To_Remove = std::remove_if(Particles_Data.begin(), Particles_Data.end(), Smoke_Particle_Remove_If);

		Particles_Data.erase(Particles_To_Remove, Particles_Data.end());
	}
};

template<typename Particle, typename Vertex_Buffer>
	class Particle_Renderer
	{
	public:
		Shader Shader;
		Particle Particles;
		Vertex_Buffer Mesh;
		Texture Albedo;
		Texture Material;

		void Update()
		{
			Particles.Update();	// This will update all of the particles to account for the new delta time
		}
		void Render()
		{
			Shader.Activate();

			Player_Camera.Bind_Buffers(Particles.Camera_Location);
			Light_Uniforms.Update_Buffer(Particles.Light_Location);

			Mesh.Bind_Buffer();

			Albedo.Parse_Texture(Shader, "Albedo", 0);
			Albedo.Bind_Texture();

			Material.Parse_Texture(Shader, "Material", 1);
			Material.Bind_Texture();

			for (size_t W = 0; W < Particles.Particles_Data.size(); W += Particles.Particles_Per_Call)
			{
				size_t Count = Particles.Parse_Values(W);

				glDrawElementsInstanced(GL_TRIANGLES, Mesh.Indices_Count, GL_UNSIGNED_INT, 0, Count);
			}
		}
	};
template<typename Particle, typename Vertex_Buffer>
	void Create_Particle_Renderer(Shader Shader, Vertex_Buffer Mesh, Texture Albedo, Texture Material, Particle_Renderer<Particle, Vertex_Buffer>* Target_Renderer)
	{
		Target_Renderer->Shader = Shader;
		Target_Renderer->Mesh = Mesh;
		Target_Renderer->Albedo = Albedo;
		Target_Renderer->Material = Material;

		Target_Renderer->Particles.Init_Shader_Information(Shader);
	}

Particle_Renderer<Smoke_Particle_Info, Model_Vertex_Buffer> Smoke_Particles;

Particle_Renderer<Smoke_Particle_Info, Billboard_Vertex_Buffer> Billboard_Smoke_Particles;

#endif
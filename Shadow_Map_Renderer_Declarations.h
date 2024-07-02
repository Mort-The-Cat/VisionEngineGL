#ifndef SHADOW_MAP_RENDERER_DECLARATIONS
#define SHADOW_MAP_RENDERER_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Model_Declarations.h"

namespace Shadow_Mapper	// I am going a little insane trying to get the render buffer working at all so I'm gonna just do a little test to see if I can get it rendering to a regular 2d framebuffer before anything fancy
{
	constexpr const bool Shadow_Mapping = true;

	unsigned int Shadow_Frame_Buffer;

	const unsigned int Shadow_Map_Width = 256u, Shadow_Map_Height = 256u;

	Shader Shadow_Object_Shader;

	Model_Uniform_Location_Object Shadow_Model_Uniform_Locations;

	glm::mat4 Shadow_Projection_Matrix;

	const float Shadow_Map_Far_Plane = 25.0f;
}

class Shadow_Map
{
public:
	unsigned int Shadow_Cubemap;

	// This will also include projection matrices for shadows

	std::array<glm::mat4, 6> View_Matrices;
	glm::vec3 Light_Position;

	Shadow_Map()
	{

	}

	void Create_Buffer()
	{
		glGenTextures(1, &Shadow_Cubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Shadow_Cubemap);

		for (size_t W = 0; W < 6; W++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + W, 0u, GL_DEPTH_STENCIL, 
				Shadow_Mapper::Shadow_Map_Width, Shadow_Mapper::Shadow_Map_Height, 0u, 
				GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, Shadow_Cubemap, 0u);
	}

	void Update_View_Matrices(glm::vec3 Position)
	{
		View_Matrices[0] = Shadow_Mapper::Shadow_Projection_Matrix * glm::lookAt(Position, Position + glm::vec3( 1.f, 0.f, 0.f), glm::vec3( 0.f,-1.f, 0.f));
		View_Matrices[1] = Shadow_Mapper::Shadow_Projection_Matrix * glm::lookAt(Position, Position + glm::vec3(-1.f, 0.f, 0.f), glm::vec3( 0.f,-1.f, 0.f));
		View_Matrices[2] = Shadow_Mapper::Shadow_Projection_Matrix * glm::lookAt(Position, Position + glm::vec3( 0.f, 1.f, 0.f), glm::vec3( 0.f, 0.f, 1.f));
		View_Matrices[3] = Shadow_Mapper::Shadow_Projection_Matrix * glm::lookAt(Position, Position + glm::vec3( 0.f,-1.f, 0.f), glm::vec3( 0.f, 0.f,-1.f));
		View_Matrices[4] = Shadow_Mapper::Shadow_Projection_Matrix * glm::lookAt(Position, Position + glm::vec3( 0.f, 0.f, 1.f), glm::vec3( 0.f,-1.f, 0.f));
		View_Matrices[5] = Shadow_Mapper::Shadow_Projection_Matrix * glm::lookAt(Position, Position + glm::vec3( 0.f, 0.f,-1.f), glm::vec3( 0.f,-1.f, 0.f));

		Light_Position = Position;
	}
};

#define NUMBER_OF_SHADOW_MAPS 1u

namespace Shadow_Mapper
{
	std::vector<Shadow_Map> Shadow_Maps;

	unsigned int Test_Attachment;

	void Bind_Shadow_Maps(Shader& Shader)
	{
		for (size_t W = 0; W < NUMBER_OF_SHADOW_MAPS; W++)
		{
			glUniform1i(glGetUniformLocation(Shader.Program_ID, "Shadow_Maps"), 1u + W);
			glActiveTexture(GL_TEXTURE1 + W);
			glBindTexture(GL_TEXTURE_CUBE_MAP, Shadow_Maps[W].Shadow_Cubemap);
		}
	}

	void Render_Object_To_Shadow_Map(Model* Object)
	{
		Object->Mesh.Bind_Buffer();
		// Object->Mesh.Update_Buffer();

		Object->Uniforms.Update_Buffer(Model_Uniform_Location);

		// Object->Uniforms.Model_Matrix = Direction_Matrix_Calculate(Object->Position, Object->Orientation, Object->Orientation_Up);

		// Object->Uniforms.Update_Buffer(Shadow_Model_Uniform_Locations);

		glDrawElements(GL_TRIANGLES, Object->Mesh.Indices_Count, GL_UNSIGNED_INT, 0u);
	}

	void Initialise_Shadow_Mapper()
	{
		glGenFramebuffers(1, &Shadow_Frame_Buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, Shadow_Frame_Buffer);

		Shadow_Object_Shader.Create_Shader("Shader_Code/Shadow_Test_2.vert", "Shader_Code/Shadow_Test_2.frag", "Shader_Code/Shadow_Test_2.geom");
		Shadow_Object_Shader.Activate();

		// Model_Vertex_Buffer_Vertex_Attributes_Set();

		Shadow_Model_Uniform_Locations = Initialise_Model_Uniform_Locations_Object(Shadow_Object_Shader);
		
		Shadow_Projection_Matrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, Shadow_Map_Far_Plane);

		Shadow_Maps.push_back(Shadow_Map());

		Shadow_Maps[0].Create_Buffer();
	}

	void Bind_Shadow_Frame_Buffer()
	{
		glViewport(0, 0, Shadow_Map_Width, Shadow_Map_Height);
		glBindFramebuffer(GL_FRAMEBUFFER, Shadow_Frame_Buffer);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}

	void Unbind_Shadow_Frame_Buffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0u);
		glViewport(0, 0, Window_Width, Window_Height);

		// glCullFace(GL_FRONT);
	}

	void Render_All_Shadows()
	{
		Bind_Shadow_Frame_Buffer();

		Scene_Object_Shader.Activate();

		// Shadow_Object_Shader.Activate();

		for (size_t Face = 0; Face < 6; Face++)
		{
			// glUniformMatrix4fv(glGetUniformLocation(Shadow_Object_Shader.Program_ID, "Projection_Matrix"), 1, GL_FALSE, &Shadow_Maps[0].View_Matrices[Face][0][0]);

			glUniformMatrix4fv(glGetUniformLocation(Scene_Object_Shader.Program_ID, "Projection_Matrix"), 1, GL_FALSE, &Shadow_Maps[0].View_Matrices[Face][0][0]);

			glBindTexture(GL_TEXTURE_CUBE_MAP, Shadow_Maps[0].Shadow_Cubemap);

			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, Shadow_Maps[0].Shadow_Cubemap, 0u);

			glClear(GL_DEPTH_BUFFER_BIT);	// For shadow mapping, we only care about the depth buffer

			// Shadow_Object_Shader.Activate();

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << " >> Framebuffer is not complete!" << std::endl;

			for (size_t W = 0; W < Scene_Models.size(); W++)
				Render_Object_To_Shadow_Map(Scene_Models[W]);

		}

		Unbind_Shadow_Frame_Buffer();
	}
}

#endif

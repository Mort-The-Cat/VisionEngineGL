#ifndef SHADOW_MAP_RENDERER_DECLARATIONS
#define SHADOW_MAP_RENDERER_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Model_Declarations.h"

namespace Shadow_Mapper
{
	constexpr const bool Shadow_Mapping = true;

	unsigned int Shadow_Frame_Buffer;

	unsigned int Shadow_Depth_Render_Buffer;	// Gah it seems redundant to do this for a shadow-map frame buffer, but I just wanna get the shadow-map working haha

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

	void Create_Buffer()
	{
		glGenTextures(1, &Shadow_Cubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Shadow_Cubemap);

		// glBindTexture(GL_TEXTURE_2D, Shadow_Cubemap);

		// glTexImage2D(GL_TEXTURE_2D, 0u, GL_R32F, Shadow_Mapper::Shadow_Map_Width, Shadow_Mapper::Shadow_Map_Height, 0u, GL_RED, GL_FLOAT, NULL);

		for (size_t W = 0; W < 6; W++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + W, 0u, GL_DEPTH_COMPONENT24, Shadow_Mapper::Shadow_Map_Width, Shadow_Mapper::Shadow_Map_Height, 0u, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP, Shadow_Cubemap, 0u, 0u);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Shadow_Cubemap, 0u);
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

	void Set_Shadow_Map(Shader& Shader, Shadow_Map& Map)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, Map.Shadow_Cubemap);

		glFramebufferTexture(GL_TEXTURE_CUBE_MAP, GL_DEPTH_ATTACHMENT, Map.Shadow_Cubemap, 0u);

		glDrawBuffer(GL_NONE);

		// glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP, Map.Shadow_Cubemap, 0u, 0u);

		//const unsigned int Buffers[] = { GL_DEPTH_ATTACHMENT };
		//glDrawBuffers(1, Buffers);

		glClear(GL_DEPTH_BUFFER_BIT);

		GLint Location = glGetUniformLocation(Shader.Program_ID, "Shadow_Matrix");
		glUniformMatrix4fv(Location, 6, GL_FALSE, &Map.View_Matrices[0][0][0]);

		glUniform3f(glGetUniformLocation(Shader.Program_ID, "Light_Position"), Map.Light_Position.x, Map.Light_Position.y, Map.Light_Position.z);
	}

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

		Object->Uniforms.Update_Buffer(Shadow_Model_Uniform_Locations);

		glDrawElements(GL_TRIANGLES, Object->Mesh.Indices_Count, GL_UNSIGNED_INT, 0u);
	}

	void Initialise_Shadow_Mapper()
	{
		Shadow_Object_Shader.Create_Shader("Shader_Code/Shadow_Test.vert", "Shader_Code/Shadow_Test.frag", "Shader_Code/Shadow_Test.geom");

		Shadow_Object_Shader.Activate();

		glGenFramebuffers(1, &Shadow_Frame_Buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, Shadow_Frame_Buffer);

		//

		for (size_t W = 0; W < NUMBER_OF_SHADOW_MAPS; W++)
		{
			Shadow_Maps.push_back(Shadow_Map());
			Shadow_Maps.back().Create_Buffer();
		}

		Set_Shadow_Map(Shadow_Object_Shader, Shadow_Maps[0]);

		glDrawBuffer(GL_NONE);

		//glGenRenderbuffers(1, &Shadow_Depth_Render_Buffer);
		//glBindRenderbuffer(GL_RENDERBUFFER, Shadow_Depth_Render_Buffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Shadow_Map_Width, Shadow_Map_Height);

		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Shadow_Depth_Render_Buffer);

		Shadow_Model_Uniform_Locations = Initialise_Model_Uniform_Locations_Object(Shadow_Object_Shader);

		glUniform1f(glGetUniformLocation(Shadow_Object_Shader.Program_ID, "Inverse_Far_Plane"), 1.0f / Shadow_Map_Far_Plane);

		Shadow_Projection_Matrix = glm::perspective(glm::radians(90.0f), (float)Shadow_Map_Width / (float)Shadow_Map_Height, 0.1f, Shadow_Map_Far_Plane);
	}

	void Bind_Shadow_Frame_Buffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, Shadow_Frame_Buffer);									// We've already bound the depth render buffer to this framebuffer so it should be alright
		glViewport(0, 0, Shadow_Mapper::Shadow_Map_Width, Shadow_Mapper::Shadow_Map_Height);

		glCullFace(GL_FRONT);
	}

	void Unbind_Shadow_Frame_Buffer()
	{
		glViewport(0, 0, Window_Width, Window_Height);
		glCullFace(GL_BACK);
	}

	void Render_All_Shadows()
	{
		Bind_Shadow_Frame_Buffer();

		Shadow_Object_Shader.Activate();

		for (size_t W = 0; W < NUMBER_OF_SHADOW_MAPS; W++)
		{
			Set_Shadow_Map(Shadow_Object_Shader, Shadow_Maps[W]);
	
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << " >> Framebuffer is not complete!" << std::endl;

			for (size_t V = 0; V < Scene_Models.size(); V++)
				if (Scene_Models[V]->Flags[MF_CAST_SHADOWS])		// If the object is flagged to cast shadows,
					Render_Object_To_Shadow_Map(Scene_Models[V]);	// render its cross-section to the shadow-map*/

			/*for (size_t Face = 0; Face < 6; Face++)							// Iterates through shadow map faces
			{
				Bind_New_Shadow_Map_Face(Shadow_Object_Shader, Face, Shadow_Maps[W]);	// For this specific shadow map face, clear the depth render buffer usw.

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << " >> Framebuffer is not complete!" << std::endl;

				for (size_t V = 0; V < Scene_Models.size(); V++)
					if (Scene_Models[V]->Flags[MF_CAST_SHADOWS])
						Render_Object_To_Shadow_Map(Scene_Models[V]);	
			}*/
		}

		Unbind_Shadow_Frame_Buffer();
	}
}

#endif

#ifndef SHADOW_MAP_RENDERER_DECLARATIONS
#define SHADOW_MAP_RENDERER_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Model_Declarations.h"

namespace Shadow_Mapper
{
	constexpr const bool Shadow_Mapping = true;

	unsigned int Shadow_Frame_Buffer;

	unsigned int Shadow_Depth_Buffer;	// Gah it seems redundant to do this for a shadow-map frame buffer, but I just wanna get the shadow-map working haha

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

		for (size_t W = 0; W < 6; W++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + W, 0u, GL_R32F, Shadow_Mapper::Shadow_Map_Width, Shadow_Mapper::Shadow_Map_Height, 0u, GL_RED, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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

	void Prepare_For_Rendering(Shader& Shader)	// call this function before the "render shadow map" function is called
	{
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Shadow_Cubemap, GL_TEXTURE_CUBE_MAP, 0u); 
		 
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Shadow_Cubemap, 0u);

		glBindTexture(GL_TEXTURE_CUBE_MAP, Shadow_Cubemap);

		for (size_t W = 5; W <= 5; W--)	// This iterates through every face of the cubemap, clearing the depth attachments
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + W, Shadow_Cubemap, 0u);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glClearColor((float)W, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// This clears the depth buffer of the shadow-map
		}

		// This the glframebuffertexture on the first face of the cubemap

		GLint Location = glGetUniformLocation(Shader.Program_ID, "Shadow_Matrix");
		glUniformMatrix4fv(Location, 1, GL_FALSE, &View_Matrices[0][0][0]);

		// GLint Location = glGetUniformLocation(Shader.Program_ID, "Shadow_Matrices");
		// glUniformMatrix4fv(Location, 6, GL_FALSE, &View_Matrices[0][0][0]);

		glUniform3f(glGetUniformLocation(Shader.Program_ID, "Light_Position"), Light_Position.x, Light_Position.y, Light_Position.z);
	}
};

#define NUMBER_OF_SHADOW_MAPS 1u

namespace Shadow_Mapper
{
	std::vector<Shadow_Map> Shadow_Maps;

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
		glGenFramebuffers(1, &Shadow_Frame_Buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, Shadow_Frame_Buffer);

		// Then, we need to initialise the shadow shader

		Shadow_Object_Shader.Create_Shader("Shader_Code/Shadow_Test.vert", "Shader_Code/Shadow_Test.frag", nullptr);

		Shadow_Object_Shader.Activate();

		Shadow_Model_Uniform_Locations = Initialise_Model_Uniform_Locations_Object(Shadow_Object_Shader);

		glUniform1f(glGetUniformLocation(Shadow_Object_Shader.Program_ID, "Inverse_Far_Plane"), 1.0f / Shadow_Map_Far_Plane);

		Shadow_Projection_Matrix = glm::perspective(glm::radians(90.0f), (float)Shadow_Map_Width / (float)Shadow_Map_Height, 0.01f, Shadow_Map_Far_Plane);

		//

		for (size_t W = 0; W < NUMBER_OF_SHADOW_MAPS; W++)
		{
			Shadow_Maps.push_back(Shadow_Map());
			Shadow_Maps.back().Create_Buffer();
		}
	}

	void Bind_Shadow_Frame_Buffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, Shadow_Frame_Buffer);
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

		glDisable(GL_DEPTH_TEST);

		Shadow_Object_Shader.Activate();

		for (size_t W = 0; W < NUMBER_OF_SHADOW_MAPS; W++)
		{
			Shadow_Maps[W].Prepare_For_Rendering(Shadow_Object_Shader);	// Set this specific shadow map

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << " >> Framebuffer is not complete!" << std::endl;

			for (size_t V = 0; V < Scene_Models.size(); V++)
				if (Scene_Models[V]->Flags[MF_CAST_SHADOWS])		// If the object is flagged to cast shadows,
					Render_Object_To_Shadow_Map(Scene_Models[V]);	// render its cross-section to the shadow-map
		}

		glEnable(GL_DEPTH_TEST);

		Unbind_Shadow_Frame_Buffer();
	}
}

#endif

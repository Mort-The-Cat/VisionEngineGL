#ifndef VISION_POST_PROCESSOR_DECLARATIONS
#define VISION_POST_PROCESSOR_DECLARATIONS

#include "Vertex_Buffer_Declarations.h"
#include "OpenGL_Declarations.h"

#include "Lighting_Handler.h"

#include "Shadow_Map_Renderer_Declarations.h"

constexpr const bool Post_Processing = true;

namespace Post_Processor
{
	Billboard_Vertex_Buffer Vertex_Buffer;

	Light_Uniform_Location_Object Deferred_Lighting_Uniform_Locations;

	Camera_Uniform_Location_Object Camera_Uniform_Locations;

	unsigned int Frame_Buffer_ID;

	unsigned int Frame_Buffer_Texture;

	//

	unsigned int Position_Buffer_Texture;

	unsigned int Normal_Buffer_Texture;

	unsigned int Material_Buffer_Texture;

	//

	unsigned int Render_Buffer_ID;

	Shader Shader_Program;

	void Delete_Buffers()
	{
		glDeleteFramebuffers(1, &Frame_Buffer_ID);

		//

		glDeleteTextures(1, &Frame_Buffer_Texture);

		glDeleteTextures(1, &Position_Buffer_Texture);

		glDeleteTextures(1, &Normal_Buffer_Texture);

		glDeleteTextures(1, &Material_Buffer_Texture);

		//

		glDeleteRenderbuffers(1, &Render_Buffer_ID);
	}

	void Create_Buffer(unsigned int& Texture, unsigned int Colour_Attachment, unsigned int Internal_Format, unsigned int Type)
	{
		glGenTextures(1, &Texture);
		glBindTexture(GL_TEXTURE_2D, Texture);

		glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, Window_Width, Window_Height, 0, GL_RGBA, Type, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, Colour_Attachment, GL_TEXTURE_2D, Texture, 0);
	}

	void Create_Buffers()
	{
		glGenFramebuffers(1, &Frame_Buffer_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_ID);

		Create_Buffer(Frame_Buffer_Texture, GL_COLOR_ATTACHMENT0, GL_RGBA8, GL_UNSIGNED_BYTE);
		Create_Buffer(Position_Buffer_Texture, GL_COLOR_ATTACHMENT1, GL_RGBA32F, GL_FLOAT);
		Create_Buffer(Normal_Buffer_Texture, GL_COLOR_ATTACHMENT2, GL_RGBA32F, GL_FLOAT);
		Create_Buffer(Material_Buffer_Texture, GL_COLOR_ATTACHMENT3, GL_RGBA8, GL_UNSIGNED_BYTE);

		//

		glGenRenderbuffers(1, &Render_Buffer_ID);
		glBindRenderbuffer(GL_RENDERBUFFER, Render_Buffer_ID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window_Width, Window_Height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Render_Buffer_ID);

		const unsigned int Buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, Buffers);

		//

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << " >> Framebuffer is not complete!" << std::endl;
	}

	void Initialise_Post_Processor()	// Only call this once. In order to merely update the post-processor, call the update function
	{
		Shader_Program.Create_Shader("Shader_Code/Post_Processing.vert", "Shader_Code/Post_Processing.frag", nullptr);

		Shader_Program.Activate();

		Deferred_Lighting_Uniform_Locations = Initialise_Light_Uniform_Locations_Object(Shader_Program);

		Camera_Uniform_Locations = Initialise_Camera_Uniform_Locations_Object(Shader_Program);

		Create_Buffers();

		//

		glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture);
		glUniform1i(glGetUniformLocation(Shader_Program.Program_ID, "Screen_Texture"), 0);
		glActiveTexture(0);

		//

		Vertex_Buffer = Billboard_Vertex_Buffer(-1, -1, 1, 1); // These are view-space screen coordinates

		// No transformation needs to be done to these at all
	}

	void Start_Rendering()	// This prepares the appropriate frame buffers for all of the rendering
	{
		glDepthMask(GL_TRUE);
		glClearColor(0.2, 0.3, 0.2, 1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_ID);
	}

	void Bind_G_Buffer_Textures(Shader Shader_Program)
	{
		glUniform1i(glGetUniformLocation(Shader_Program.Program_ID, "Screen_Texture"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture); // This is important for getting the screen texture

		//

		glUniform1i(glGetUniformLocation(Shader_Program.Program_ID, "Position_Texture"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Position_Buffer_Texture);

		glUniform1i(glGetUniformLocation(Shader_Program.Program_ID, "Normal_Texture"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Normal_Buffer_Texture); // This is important for getting the screen texture

		glUniform1i(glGetUniformLocation(Shader_Program.Program_ID, "Material_Texture"), 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Material_Buffer_Texture); // This is important for getting the screen texture
	}

	void Finish_Rendering()	// This takes what has been rendered and redraws it on the screen with a quick post-processing pass
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0u);

		//

		Shader_Program.Activate();

		Vertex_Buffer.Bind_Buffer();

		Player_Camera.Bind_Buffers(Camera_Uniform_Locations);

		Test_Cubemap.Parse_Texture(Shader_Program, "Cubemap", 0);
		Test_Cubemap.Bind_Texture();

		if (Shadow_Mapper::Shadow_Mapping)
			Shadow_Mapper::Bind_Shadow_Maps(Shader_Program);

		Light_Uniforms.Update_Buffer(Deferred_Lighting_Uniform_Locations);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		
		Bind_G_Buffer_Textures(Shader_Program);

		glDrawElements(GL_TRIANGLES, Vertex_Buffer.Indices_Count, GL_UNSIGNED_INT, 0); // This does the draw call

		//

		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, Frame_Buffer_ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0u);
		glBlitFramebuffer(0, 0, Window_Width, Window_Height, 0, 0, Window_Width, Window_Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		// This is necessary so that the depth-buffer can be used while rendering particles to the default framebuffer
	}
}

#endif
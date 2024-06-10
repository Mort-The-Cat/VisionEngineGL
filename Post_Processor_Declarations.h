#ifndef VISION_POST_PROCESSOR_DECLARATIONS
#define VISION_POST_PROCESSOR_DECLARATIONS

#include "Vertex_Buffer_Declarations.h"
#include "OpenGL_Declarations.h"

namespace Post_Processor
{
	Billboard_Vertex_Buffer Vertex_Buffer;

	unsigned int Frame_Buffer_ID;

	unsigned int Frame_Buffer_Texture;

	unsigned int Render_Buffer_ID;

	Shader Shader_Program;

	void Initialise_Post_Processor()	// Only call this once. In order to merely update the post-processor, call the update function
	{
		Shader_Program.Create_Shader("Shader_Code/Post_Processing.vert", "Shader_Code/Post_Processing.frag", nullptr);

		Shader_Program.Activate();

		glGenFramebuffers(1, &Frame_Buffer_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_ID);

		glGenTextures(1, &Frame_Buffer_Texture);
		glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Window_Width, Window_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Frame_Buffer_Texture, 0);

		// GLenum Draw_Buffers[] = { GL_COLOR_ATTACHMENT0 };

		// glDrawBuffers(1, Draw_Buffers);

		const unsigned int Buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, Buffers);

		//

		glGenRenderbuffers(1, &Render_Buffer_ID);
		glBindRenderbuffer(GL_RENDERBUFFER, Render_Buffer_ID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window_Width, Window_Height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Render_Buffer_ID);

		//

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << " >> Framebuffer is not complete!" << std::endl;

		//

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

	void Finish_Rendering()	// This takes what has been rendered and redraws it on the screen with a quick post-processing pass
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0u);

		//

		Shader_Program.Activate();

		Vertex_Buffer.Bind_Buffer();

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		// glDisable(GL_CULL_FACE);

		
		glUniform1i(glGetUniformLocation(Shader_Program.Program_ID, "Screen_Texture"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture); // This is important for getting the screen texture

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
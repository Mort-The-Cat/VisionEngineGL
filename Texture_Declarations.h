#ifndef TEXTURE_DECLARATIONS_OPENGL
#define TEXTURE_DECLARATIONS_OPENGL

#include "OpenGL_Declarations.h"

class Texture
{
	unsigned int Texture_ID = 0xFFFFFFFF;
public:
	Texture() {}

	void Bind_Texture()
	{
		glBindTexture(GL_TEXTURE_2D, Texture_ID);
	}

	void Create_Texture()
	{
		glGenTextures(1, &Texture_ID);

		Bind_Texture();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//

		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
		// glGenerateMipmap(GL_TEXTURE_2D);
	}
};

#endif
#ifndef TEXTURE_DECLARATIONS_OPENGL
#define TEXTURE_DECLARATIONS_OPENGL

#include "OpenGL_Declarations.h"

class Texture
{
	unsigned int Texture_ID = Unassigned_Bit_Mask;
public:
	Texture() {}

	void Delete_Texture()
	{
		if(Texture_ID != Unassigned_Bit_Mask) // We only want to delete this texture if we've created the gl texture id
			glDeleteTextures(1, &Texture_ID);
	}

	void Parse_Texture(Shader Shader, const char* Location, GLuint Unit)
	{
		GLint Uniform_Location = glGetUniformLocation(Shader.Program_ID, Location);

		glUniform1i(Uniform_Location, Unit);
		glActiveTexture(GL_TEXTURE0 + Unit);
	}

	void Bind_Texture()
	{

		// Texture_Units++;
		glBindTexture(GL_TEXTURE_2D, Texture_ID);
	}

	void Create_Texture()
	{
		glGenTextures(1, &Texture_ID);

		Bind_Texture();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//

		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
		// glGenerateMipmap(GL_TEXTURE_2D);
	}
};

class Array_Texture
{
	unsigned int Texture_ID = Unassigned_Bit_Mask;
public:
	Array_Texture() {}

	void Delete_Texture()
	{
		if (Texture_ID != Unassigned_Bit_Mask) // We only want to delete this texture if we've created the gl texture id
			glDeleteTextures(1, &Texture_ID);
	}

	void Parse_Texture(Shader Shader, const char* Location, GLuint Unit)
	{
		GLint Uniform_Location = glGetUniformLocation(Shader.Program_ID, Location);

		glUniform1i(Uniform_Location, Unit);
		glActiveTexture(GL_TEXTURE0 + Unit);
	}

	void Bind_Texture()
	{

		// Texture_Units++;
		glBindTexture(GL_TEXTURE_2D_ARRAY, Texture_ID);
	}

	void Create_Texture()
	{
		glGenTextures(1, &Texture_ID);

		Bind_Texture();
	}
};

class Cubemap_Texture
{
	unsigned int Texture_ID = Unassigned_Bit_Mask;
public:
	Cubemap_Texture() {}

	void Delete_Texture()
	{
		if (Texture_ID != Unassigned_Bit_Mask)
			glDeleteTextures(1, &Texture_ID);
	}

	void Parse_Texture(Shader Shader, const char* Location, GLuint Unit)
	{
		GLint Uniform_Location = glGetUniformLocation(Shader.Program_ID, Location);

		glUniform1i(Uniform_Location, Unit);
		glActiveTexture(GL_TEXTURE0 + Unit);
	}

	void Bind_Texture()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, Texture_ID);
	}

	void Create_Texture()
	{
		glGenTextures(1, &Texture_ID);

		Bind_Texture();
	}
};

Cubemap_Texture Test_Cubemap;

void Load_Cubemap(std::array<const char*, 6> Faces, Cubemap_Texture* Target_Texture)
{
	int Width, Height, Channels;
	unsigned char* Data;

	stbi_set_flip_vertically_on_load(false);

	Target_Texture->Create_Texture();

	for (size_t W = 0; W < Faces.size(); W++)
	{
		Data = stbi_load(Faces[W], &Width, &Height, &Channels, 0); // We don't *need* all four channels, but GPUs handle 4 channel images best

		if (Data)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + W,
				0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

			stbi_image_free(Data);
		}
		else
			Throw_Error(" >> Unable to load cubemap image!\n");
	}

	stbi_set_flip_vertically_on_load(false);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

#endif
#ifndef ASSET_LOADING_CACHE_OPENGL
#define ASSET_LOADING_CACHE_OPENGL

#include "OpenGL_Declarations.h"
#include "Texture_Declarations.h"

#define LOAD_TEXTURE_SEARCH_CACHE_BIT 1u
#define LOAD_TEXTURE_STORE_CACHE_BIT 2u

#define LOAD_MESH_SEARCH_CACHE_BIT 4u
#define LOAD_MESH_STORE_CACHE_BIT 8u
#define LOAD_MESH_CREATE_BUFFERS_CACHE_BIT 16u

namespace Cache
{
	struct Texture_Cache_Info
	{
		alignas(0) std::string Directory;
		stbi_uc* Pixels;
		int Texture_Width, Texture_Height, Texture_Channels;
		Texture Texture;
	};
}

void Get_Texture(std::string Directory, Texture* Target_Texture)
{
	Target_Texture->Create_Texture();

	int Width, Height, Channels;

	unsigned char* Pixels = stbi_load(Directory.c_str(), &Width, &Height, &Channels, 0);

	if (Pixels)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		Throw_Error(" >> Unable to load texture!");

	stbi_image_free(Pixels);
}

#endif
#ifndef ASSET_LOADING_CACHE_OPENGL
#define ASSET_LOADING_CACHE_OPENGL

#include "OpenGL_Declarations.h"
#include "Texture_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Mesh_Loader.h"

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

		Texture Texture;	// This can be taken easily from the cache
	};

	struct Mesh_Cache_Info
	{
		alignas(0) std::string Directory;
		Model_Mesh* Mesh; // This is saved in the cache for later use.

		Model_Vertex_Buffer Vertex_Buffer;

		// However, we must deallocate the mesh data before we remove this item from the mesh cache!!
	};

	std::vector<Mesh_Cache_Info> Mesh_Cache;
	std::vector<Texture_Cache_Info> Texture_Cache;

	void Clear_Texture_Cache()
	{
		for (size_t W = 0; W < Texture_Cache.size(); W++)
		{
			Texture_Cache[W].Texture.Delete_Texture();
			stbi_image_free(Texture_Cache[W].Pixels);
		}
		Texture_Cache.clear();
	}

	void Clear_Mesh_Cache()
	{
		for (size_t W = 0; W < Mesh_Cache.size(); W++)
		{
			Mesh_Cache[W].Vertex_Buffer.Delete_Buffer();
			delete Mesh_Cache[W].Mesh;
		}
		Mesh_Cache.clear();
	}

	bool Search_Texture_Cache(const char* Directory, Texture_Cache_Info* Target_Info)
	{
		for (size_t W = 0; W < Texture_Cache.size(); W++)
		{
			if (strcmp(Directory, Texture_Cache[W].Directory.c_str()) == 0)
			{
				*Target_Info = Texture_Cache[W];
				return true;
			}
		}
		return false;
	}

	bool Search_Mesh_Cache(const char* Directory, Mesh_Cache_Info* Target_Info)
	{
		for (size_t W = 0; W < Mesh_Cache.size(); W++)
		{
			if (strcmp(Directory, Mesh_Cache[W].Directory.c_str()) == 0)
			{
				*Target_Info = Mesh_Cache[W];
				return true;
			}
		}
		return false;
	}
}

void Load_New_Texture(std::string Directory, Cache::Texture_Cache_Info* Target_Info)
{
	Target_Info->Texture.Create_Texture();

	stbi_set_flip_vertically_on_load(false);

	Target_Info->Pixels = stbi_load(Directory.c_str(), &Target_Info->Texture_Width, &Target_Info->Texture_Height, &Target_Info->Texture_Channels, 0);

	if (Target_Info->Pixels)
	{
		auto Channel_Flag = Target_Info->Texture_Channels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Target_Info->Texture_Width, Target_Info->Texture_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Target_Info->Pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		Throw_Error(" >> Unable to load texture!");

	Target_Info->Directory = Directory;

	// stbi_image_free(Pixels);
}

Cache::Texture_Cache_Info Pull_Texture(const char* Texture_Directory)
{
	Cache::Texture_Cache_Info Cache_Info;

	//if (Flags & LOAD_TEXTURE_SEARCH_CACHE_BIT)
	//{
	if (Cache::Search_Texture_Cache(Texture_Directory, &Cache_Info))
		return Cache_Info;
	//}

	Load_New_Texture(Texture_Directory, &Cache_Info);

	Cache::Texture_Cache.push_back(Cache_Info);
	return Cache_Info;
}

//

Cache::Mesh_Cache_Info Pull_Mesh(const char* Directory)
{
	Cache::Mesh_Cache_Info Cache_Info;

	if (Cache::Search_Mesh_Cache(Directory, &Cache_Info))
		return Cache_Info;

	Cache_Info.Mesh = new Model_Mesh(); // This allocates the memory that is used by the load_mesh_obj function

	Load_Mesh_Obj(Directory, Cache_Info.Mesh);

	Cache_Info.Directory = Directory;
	Cache_Info.Vertex_Buffer.Create_Buffer();
	Cache_Info.Vertex_Buffer.Mesh = Cache_Info.Mesh;
	Cache_Info.Vertex_Buffer.Bind_Buffer();
	Cache_Info.Vertex_Buffer.Update_Buffer();

	Cache::Mesh_Cache.push_back(Cache_Info);
	return Cache_Info;
}

#endif
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

void Transparency(Cache::Texture_Cache_Info* Target_Info)
{
	size_t Total_Size = Target_Info->Texture_Height * Target_Info->Texture_Width * 4;

	for (size_t W = 3; W < Total_Size; W += 4)
		Target_Info->Pixels[W] = 64; // We want half the value set
}

void Load_New_Texture(std::string Directory, Cache::Texture_Cache_Info* Target_Info)
{
	Target_Info->Texture.Create_Texture();

	stbi_set_flip_vertically_on_load(false);

	Target_Info->Pixels = stbi_load(Directory.c_str(), &Target_Info->Texture_Width, &Target_Info->Texture_Height, &Target_Info->Texture_Channels, 0);

	//if (strcmp(Directory.c_str(), "Assets/Textures/Important.png") == 0)
	//	Transparency(Target_Info);

	if (Target_Info->Pixels)
	{
		auto Channel_Flag = Target_Info->Texture_Channels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Target_Info->Texture_Width, Target_Info->Texture_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Target_Info->Pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		Throw_Error(" >> Unable to load texture!");

	Target_Info->Directory = Directory;
}

void Push_Merged_Specular_Reflectivity(const char* T_1, const char* T_2, const char* Material_Name)
{
	Cache::Texture_Cache_Info Final_Texture;
	std::array<Cache::Texture_Cache_Info, 2> Textures;
	std::array<const char*, 2> Directories = { T_1, T_2 };

	for (size_t W = 0; W < 2; W++)
		Textures[W].Pixels = stbi_load(Directories[W], &Textures[W].Texture_Width, &Textures[W].Texture_Height, &Textures[W].Texture_Channels, STBI_rgb_alpha);

	size_t Total_Size = Textures[0].Texture_Width * Textures[0].Texture_Height * 4;

	Final_Texture.Pixels = (stbi_uc*)malloc(Total_Size);

	for (size_t W = 0; W < Total_Size; W += 4)
	{
		((unsigned char*)Final_Texture.Pixels)[W] = (unsigned char)Textures[0].Pixels[W];
		((unsigned char*)Final_Texture.Pixels)[W + 1] = (unsigned char)Textures[1].Pixels[W];
		((unsigned char*)Final_Texture.Pixels)[W + 2] = 128;
		((unsigned char*)Final_Texture.Pixels)[W + 3] = 128;
	}

	Final_Texture.Texture_Channels = Textures[0].Texture_Channels;
	Final_Texture.Texture_Width = Textures[0].Texture_Width;
	Final_Texture.Texture_Height = Textures[0].Texture_Height;

	Final_Texture.Directory = Material_Name;

	for (size_t W = 0; W < 2; W++)
		stbi_image_free(Textures[W].Pixels);

	Final_Texture.Texture.Create_Texture();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Final_Texture.Texture_Width, Final_Texture.Texture_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Final_Texture.Pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	Cache::Texture_Cache.push_back(Final_Texture);
}

void Push_Merged_Material(const char* T_1, const char* T_2, const char* T_3, const char* Material_Name)
{
	Cache::Texture_Cache_Info Final_Texture;
	std::array<Cache::Texture_Cache_Info, 3> Textures;
	std::array<const char*, 3> Directories = { T_1, T_2, T_3 };

	for (size_t W = 0; W < 3; W++)
		Textures[W].Pixels = stbi_load(Directories[W], &Textures[W].Texture_Width, &Textures[W].Texture_Height, &Textures[W].Texture_Channels, STBI_rgb_alpha);

	size_t Total_Size = Textures[0].Texture_Width * Textures[0].Texture_Height * 4;

	Final_Texture.Pixels = (stbi_uc*)malloc(Total_Size);

	for (size_t W = 0; W < Total_Size; W += 4)
	{
		((char*)Final_Texture.Pixels)[W] = (char)Textures[0].Pixels[W];
		((char*)Final_Texture.Pixels)[W + 1] = (char)Textures[1].Pixels[W];
		((char*)Final_Texture.Pixels)[W + 2] = (char)Textures[2].Pixels[W + 1];
		((char*)Final_Texture.Pixels)[W + 3] = (char)Textures[2].Pixels[W + 0];
	}

	Final_Texture.Texture_Channels = Textures[0].Texture_Channels;
	Final_Texture.Texture_Width = Textures[0].Texture_Width;
	Final_Texture.Texture_Height = Textures[0].Texture_Height;

	Final_Texture.Directory = Material_Name;

	for (size_t W = 0; W < 3; W++)
		stbi_image_free(Textures[W].Pixels);

	Final_Texture.Texture.Create_Texture();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Final_Texture.Texture_Width, Final_Texture.Texture_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Final_Texture.Pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	Cache::Texture_Cache.push_back(Final_Texture);
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
#ifndef DELETION_HANDLER_VISIONGL
#define DELETION_HANDLER_VISIONGL

#include "OpenGL_Declarations.h"
#include "Lighting_Handler.h"
#include "Model_Declarations.h"

bool Is_Deleted(void* Pointer)
{
	return Pointer == nullptr;
}

void Handle_Deletions()
{
	for (size_t W = 0; W < Scene_Lights.size(); W++)
	{
		if (Scene_Lights[W]->Flags[LF_TO_BE_DELETED])
		{
			delete Scene_Lights[W];
			Scene_Lights[W] = nullptr;
		}
	}

	auto Deleted_Lights = std::remove_if(Scene_Lights.begin(), Scene_Lights.end(), Is_Deleted);
	Scene_Lights.erase(Deleted_Lights, Scene_Lights.end());

	for (size_t W = 0; W < Scene_Models.size(); W++)
	{
		if (Scene_Models[W]->Flags[MF_TO_BE_DELETED])
		{
			delete Scene_Models[W];
			Scene_Models[W] = nullptr;
		}
	}

	auto Deleted_Models = std::remove_if(Scene_Models.begin(), Scene_Models.end(), Is_Deleted);
	Scene_Models.erase(Deleted_Models, Scene_Models.end());
}

#endif
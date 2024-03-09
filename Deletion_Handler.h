#ifndef DELETION_HANDLER_VISIONGL
#define DELETION_HANDLER_VISIONGL

#include "OpenGL_Declarations.h"
#include "Lighting_Handler.h"
#include "Model_Declarations.h"
#include "Physics_Engine.h"

bool Is_Deleted(void* Pointer)
{
	return Pointer == nullptr;
}

void Handle_Deletions()
{
	Wait_On_Physics();

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

	for (size_t W = 0; W < Physics::Scene_Physics_Objects.size(); W++)
	{
		if (Physics::Scene_Physics_Objects[W]->Flags[PF_TO_BE_DELETED])
		{
			delete Physics::Scene_Physics_Objects[W];
 			Physics::Scene_Physics_Objects[W] = nullptr;
		}
	}

	auto Deleted_Physics_Objects = std::remove_if(Physics::Scene_Physics_Objects.begin(), Physics::Scene_Physics_Objects.end(), Is_Deleted);
	Physics::Scene_Physics_Objects.erase(Deleted_Physics_Objects, Physics::Scene_Physics_Objects.end());

	//

	for (size_t W = 0; W < Scene_Hitboxes.size(); W++)
	{
		if (Scene_Hitboxes[W]->Flags[HF_TO_BE_DELETED])
		{
			delete Scene_Hitboxes[W];
			Scene_Hitboxes[W] = nullptr;
		}
	}

	auto Deleted_Hitboxes = std::remove_if(Scene_Hitboxes.begin(), Scene_Hitboxes.end(), Is_Deleted);
	Scene_Hitboxes.erase(Deleted_Hitboxes, Scene_Hitboxes.end());
}

#endif
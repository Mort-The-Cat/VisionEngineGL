#ifndef MODEL_VISIONGL
#define MODEL_VISIONGL

#include "Model_Declarations.h"
#include "Physics_Engine.h"

void Create_Model(Model_Vertex_Buffer Mesh, Texture Albedo, Texture Material, Model* Target_Model, Controller* Controlp, Hitbox* Hitboxp)
{
	Target_Model->Mesh = Mesh;
	Target_Model->Albedo = Albedo;
	Target_Model->Material = Material;
	Target_Model->Control = Controlp;
	Target_Model->Hitbox = Hitboxp;

	Controlp->Initialise_Control(Target_Model);

	if (Target_Model->Flags[MF_SOLID] && Hitboxp != nullptr)
	{
		Target_Model->Hitbox->Position = &Target_Model->Position;
		Target_Model->Hitbox->Object = Target_Model;

		Scene_Hitboxes.push_back(Hitboxp);
		if (Target_Model->Flags[MF_PHYSICS_TEST] && Scene_Hitboxes.size() > Physics::Scene_Physics_Objects.size())
			std::swap(Scene_Hitboxes[Physics::Scene_Physics_Objects.size() - 1], Scene_Hitboxes.back());
	}
}

#endif

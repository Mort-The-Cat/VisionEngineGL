#ifndef VISION_ENGINE_TITLE_SCREEN_HANDLING
#define VISION_ENGINE_TITLE_SCREEN_HANDLING

#include "../Engine_Loop_Includes.h"
#include "../Engine_Loop.h"
#include "../UI_Textbox_Definitions.h"

#include "../Light_BVH_Tree_Handler.h"

void Setup_Test_Scene()
{
	// Scene_Lights.push_back(new Lightsource(glm::vec3(0, 3, -3), glm::vec3(1.5, 1, 1.3), glm::vec3(0, 0, 1), 60, 3));
	Scene_Lights.push_back(new Lightsource(glm::vec3(0, 3, -3), glm::vec3(1, 1, 1.1), glm::vec3(0, 0, 1), 60, 3, 0.6f));



	// UI_Elements.push_back(new Button_UI_Element(-0.25, -0.1, 0.55, 0.5, Return_To_Game_Loop));

	// UI_Elements.push_back(new Button_UI_Element(-0.75, -0.9, -0.3, -0.3, Return_To_Game_Loop));

	// UI_Elements.push_back(new Button_UI_Element(-1, -1, 1, 1, Return_To_Game_Loop, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture));

	
	UI_Elements.push_back(new Button_Text_UI_Element(-0.9f, -0.9, 0.8f, -0.3, Return_To_Game_Loop, "Spiel beginnen? Das  sieht schön aus!"));
	
	
	//UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;

	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true; 

	Scene_Models.push_back(new Model({ MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(0, -0.8, 0);
	Create_Model(Pull_Mesh("Assets/Models/Test_Makarov.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Arm_Texture_2.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Test_Makarov.anim"), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(0, -0.8, 0);
	Create_Model(Pull_Mesh("Assets/Models/Test_Makarov_Shoot.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Gun_Texture.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Test_Makarov_Shoot.anim"), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh));


	Scene_Models.push_back(new Model({ MF_SOLID, MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(-12, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Test_Animation.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Viking_Room.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Test_Vertex_Anims.anim"), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(-8, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Two_Bones_Test.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Viking_Room.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Two_Bones_Test.anim"), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh));

	//

	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS }));
	Scene_Models.back()->Position = glm::vec3(0, 6, 0);
	Create_Model(Pull_Mesh("Assets/Models/Test_Level.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Level.obj").Mesh));

	//

	Scene_Lights.push_back(new Lightsource(glm::vec3(-3, -4, -3), 0.5f * glm::vec3(2, 3, 4), glm::vec3(-1, 0, 0), 80, 10, 5.0f));

	// Scene_Lights.push_back(new Lightsource(glm::vec3(-5, -3, 2), 0.5f * glm::vec3(4, 3, 3), glm::normalize(glm::vec3(0.75, 0.5, 1)), 100, 0.1));

	// 554.793457, -34.912498, 0.000000

	//Scene_Models.push_back(new Model({  }));
	//Scene_Models.back()->Position = glm::vec3(7.021941, -6.984860, -3.516123);
	//Create_Model(Pull_Mesh("Assets/Models/Normalised_Cone.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Stone").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(0, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Stone").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(5, -5, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(18, -5, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor_Reflect").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

	if (false)
	{
		Scene_Models.push_back(new Model({ MF_SOLID }));
		Scene_Models.back()->Position = glm::vec3(-5, -5, -3);
		Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

		Scene_Models.push_back(new Model({ MF_SOLID }));
		Scene_Models.back()->Position = glm::vec3(0, -5, -8);
		Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

		Scene_Models.push_back(new Model({ MF_SOLID }));
		Scene_Models.back()->Position = glm::vec3(0, -5, 2);
		Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));
	}

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(0, -5, -5);
	Create_Model(Pull_Mesh("Assets/Models/Ramp.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/White.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Controller(), Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Models/Ramp.obj").Mesh));

	glm::vec3 Rand_Direction = glm::normalize(glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f));

	//for (float X = 10; X < 40; X += 10)
	//	Volumetric_Cone_Particles.Particles.Spawn_Particle(glm::vec3(7.021941, -6.984860, -3.516123 + (X - 25.0f) * 0.3), Rand_Direction, glm::vec3(0.75, 0.75, sin(X * DTR)), 1.0f, X);

	for (size_t W = 0; W < 20; W++)
	{
		Scene_Lights.push_back(new Lightsource(glm::vec3(RNG() * 10 - 5, -4.1, RNG() * 10 - 5), glm::vec3(RNG(), RNG(), RNG()), glm::vec3(0, 0, 0), 360.0f, 1.0f, 0.6f));
	}

	Scene_Lights.push_back(new Lightsource(glm::vec3(2.310351, -4.430879, 9.878356), glm::vec3(RNG(), RNG(), RNG()), glm::vec3(0, 0, 0), 360.0f, 1.0f, 0.6f));

	Lighting_BVH::Add_Light_Occluders();

	Lighting_BVH::Generate_Light_BVH_Tree();

	Lighting_BVH::Update_Leaf_Node_Data();
}

void Run_Engine_Loop(UI_Element* Element) 
{ 
	for (size_t W = 0; W < UI_Elements.size(); W++)
		UI_Elements[W]->Flags[UF_TO_BE_DELETED] = true;

	Galaxy_Particles.Delete_All_Particles();

	Setup_Test_Scene();

	Cursor_Reset = true;

	Player_Camera.Position = glm::vec3(
		-1, -1.2, -4
	);

	Engine_Loop(); 
}

void Title_Screen_Loop();

void Create_Title_Screen_Page()
{
	for (float Radius = 0.1f; Radius < 10.0f; Radius += 0.15f)
	{
		float Total = 64 * Radius;

		for (size_t W = 0; W < Total; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.3f, (W + RNG()) * 3.14159f * 2.0f / Total);

		for (size_t W = 0; W < 256 * Radius; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.1f, 6.28318f + (W + RNG() * 4.0f) * 3.14159f * 2.0f / (64 * Radius));
	}

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f));
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;

	UI_Elements.push_back(new Button_Text_UI_Element(-1.75f, -0.9, 1.75f, -0.5f, Run_Engine_Loop, "Drücken Sie diesen Knopf, um anzufangen", glm::vec3(1.0f), 0.066667f, 0.1f));
	UI_Elements.back()->Image = Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture;

	UI_Elements.back()->Flags[UF_IMAGE] = true;

	//

	UI_Elements.push_back(new Textbox_UI_Element(-1.0f, -0.25f, 1.0f, 0.75f));

	Cursor_Reset = false;

	Player_Camera.Position = glm::vec3(
		2.872747f, -1.759029f, 9.186198f
	);

	Player_Camera.Orientation = glm::vec3(
		616.330139f, -40.649986f, 0.000000f
	);

	Title_Screen_Loop();
}

void Title_Screen_Loop()
{
	while (!glfwWindowShouldClose(Window))
	{
		double Current_Time = glfwGetTime();
		Tick = Current_Time - Last_Time;
		Last_Time = Current_Time;

		glBindFramebuffer(GL_FRAMEBUFFER, 0u);


		glDepthMask(GL_TRUE);

		glClearColor(0.05f, 0.05f, 0.05f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Receive_Inputs();

		Audio::Handle_Audio(Player_Camera);

		Player_Camera.Set_Projection_Matrix();
		Player_Camera.Set_Audio_Observer();

		{
			Galaxy_Particles.Update();
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We'll be using purely additive blending for the galaxy particles
			Galaxy_Particles.Shader.Activate();
			Galaxy_Particles.Render();
		}

		Handle_UI();

		Handle_Deletions();

		End_Of_Frame();
	}
}

#endif

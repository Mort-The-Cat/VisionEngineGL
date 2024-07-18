#ifndef ENGINE_LOOP_VISION
#define ENGINE_LOOP_VISION

#include "Engine_Loop_Includes.h"

void Render_All()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	Test_Cubemap.Parse_Texture(Scene_Object_Shader, "Cubemap", 0);
	Test_Cubemap.Bind_Texture();

	Update_Lighting_Buffer();

	Scene_Lights[0]->Position = Player_Camera.Position;

	Scene_Lights[0]->Direction = Camera_Direction;

	for (size_t W = 0; W < Scene_Models.size(); W++)
	{
		Scene_Models[W]->Render(Scene_Object_Shader);
	}

	if (Shadow_Mapper::Shadow_Mapping)
		Shadow_Mapper::Render_All_Shadows();

	if(Post_Processing)
		Post_Processor::Finish_Rendering();

	// After the post-processing effects are done, these transparent particles can be drawn

	Smoke_Particles.Shader.Activate();

	glEnable(GL_BLEND);

	glDepthMask(GL_FALSE);

	Test_Cubemap.Parse_Texture(Smoke_Particles.Shader, "Cubemap", 0);
	Test_Cubemap.Bind_Texture();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Smoke_Particles.Render();

	//

	Billboard_Smoke_Particles.Shader.Activate();

	Test_Cubemap.Parse_Texture(Billboard_Smoke_Particles.Shader, "Cubemap", 0);
	Test_Cubemap.Bind_Texture();
	Billboard_Smoke_Particles.Render();

	//

	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We'll be using purely additive blending for the fire particles
	Billboard_Fire_Particles.Shader.Activate();
	Billboard_Fire_Particles.Render();

	// Galaxy_Particles.Shader.Activate();
	// Galaxy_Particles.Render();
}

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

	Scene_Models.push_back(new Model({ MF_SOLID, MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(0, -3, 0);
	Create_Model(Pull_Mesh("Assets/Models/Test_Makarov.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Arm_Texture_2.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Test_Makarov.anim"), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(0, -3, 0);
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
	Create_Model(Pull_Mesh("Assets/Models/Test_Level.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Level.obj").Mesh));

	//

	Scene_Lights.push_back(new Lightsource(glm::vec3(-3, -4, -3), 0.5f * glm::vec3(2, 3, 4), glm::vec3(-1, 0, 0), 80, 10, 5.0f));

	// Scene_Lights.push_back(new Lightsource(glm::vec3(-5, -3, 2), 0.5f * glm::vec3(4, 3, 3), glm::normalize(glm::vec3(0.75, 0.5, 1)), 100, 0.1));

	Scene_Models.push_back(new Model( { MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(0, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Stone").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(5, -5, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(18, -5, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor_Reflect").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

	if(false)
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
}

void End_Of_Frame()
{
	Frame_Counter++;

	Time_Elapsed_Since_FPS_Update += Tick;

	if (Time_Elapsed_Since_FPS_Update > 1.0f)
	{
		glfwSetWindowTitle(Window, (
			std::string("VisionEngineGL | FPS: ") + std::to_string((int)(Frame_Counter / Time_Elapsed_Since_FPS_Update))/* +
			std::string(" | Rendering + Physics resolve: (ms) ") + std::to_string(1000.0f * (glfwGetTime() - Start_Timer)) +
			std::string(" | Begin physics + update audio/particles: (ms) ") + std::to_string(1000.0f * (Start_Timer - Other_Start_Timer)*/
			).c_str());

		Time_Elapsed_Since_FPS_Update = 0;
		Frame_Counter = 0;
	}

	glfwSwapBuffers(Window);
	glfwPollEvents();
}

void Engine_Loop()
{
	Last_Time = glfwGetTime();

	while (!glfwWindowShouldClose(Window))
	{
		double Current_Time = glfwGetTime();
		Tick = (Current_Time - Last_Time);
		Last_Time = Current_Time;

		Receive_Inputs();

		Player_Movement();

		//

		if(Post_Processing)
			Post_Processor::Start_Rendering();
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0u);

		glDepthMask(GL_TRUE);

		glClearColor(0.2, 0.3, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Scene_Lights[0]->Blur = 45 + 45 * sinf(glfwGetTime());

		// double Other_Start_Timer = glfwGetTime();

		Audio::Handle_Audio(Player_Camera);

		Physics::Record_Collisions();
		
		Handle_Scene();

		Smoke_Particles.Update();
		Billboard_Smoke_Particles.Update();
		Billboard_Fire_Particles.Update();
		// Galaxy_Particles.Update();

		Scene_Object_Shader.Activate();

		Player_Camera.Set_Projection_Matrix();
		Player_Camera.Set_Audio_Observer();
		Player_Camera.Bind_Buffers(Camera_Uniform_Location);

		//

		// double Start_Timer = glfwGetTime();

		Render_All();

		glDisable(GL_DEPTH_TEST);

		Physics::Resolve_Collisions();

		Handle_UI(); // We're able to handle all of the UI whilst the collisions are resolving!

		Handle_Deletions();

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//

		End_Of_Frame();
	}
}

//

void UI_Loop()
{
	glDisable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0u);

	UI_Continue_Looping = true;

	Last_Time = glfwGetTime();

	while (UI_Continue_Looping && !glfwWindowShouldClose(Window))
	{
		double Current_Time = glfwGetTime();
		Tick = Current_Time - Last_Time;
		Last_Time = Current_Time;

		glBindFramebuffer(GL_FRAMEBUFFER, 0u);

		glClear(GL_COLOR_BUFFER_BIT);

		Receive_Inputs();

		Audio::Handle_Audio(Player_Camera);

		Handle_UI();

		Handle_Deletions();

		End_Of_Frame();
	}
}

#endif
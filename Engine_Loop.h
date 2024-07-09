#ifndef ENGINE_LOOP_VISION
#define ENGINE_LOOP_VISION

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Uniform_Buffer_Declarations.h"
#include "Texture_Declarations.h"
#include "Asset_Loading_Cache.h"
#include "Input_Handler.h"
#include "Mesh_Loader.h"
#include "Model.h"
#include "Lighting_Handler.h"
#include "Job_System.h"
#include "Deletion_Handler.h"
#include "Mesh_Animator_Declarations.h"
#include "Particle_System_Declarations.h"

#include "Audio_Handler_Declarations.h"

#include "Physics_Engine.h"

#include "Post_Processor_Declarations.h"

#include "Shadow_Map_Renderer_Declarations.h"

#include "UI_Renderer_Declarations.h"

void Initialise_Particles()
{
	Shader Smoke_Particle_Shader;
	Smoke_Particle_Shader.Create_Shader("Shader_Code/Smoke_Particle.vert", "Shader_Code/Particle.frag", "Shader_Code/Vertex_Test.geom");

	Create_Particle_Renderer(Smoke_Particle_Shader, Pull_Mesh("Assets/Models/Smoke.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Smoke_Noise.png").Texture, Pull_Texture("Black").Texture, &Smoke_Particles);

	Shader Billboard_Particle_Shader;
	Billboard_Particle_Shader.Create_Shader("Shader_Code/Billboard_Smoke_Particle.vert", "Shader_Code/Particle.frag", "Shader_Code/Vertex_Test.geom");

	Create_Particle_Renderer(Billboard_Particle_Shader, Billboard_Vertex_Buffer(-0.05, -0.05, 0.05, 0.05), Pull_Texture("Assets/Textures/Smoke.png").Texture, Pull_Texture("Black").Texture, &Billboard_Smoke_Particles);


	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We'll be using purely additive blending for the fire particles
	Shader Billboard_Fire_Shader;
	Billboard_Fire_Shader.Create_Shader("Shader_Code/Billboard_Fire_Particle.vert", "Shader_Code/Particle.frag", "Shader_Code/Vertex_Test.geom");

	Create_Particle_Renderer(Billboard_Fire_Shader, Billboard_Vertex_Buffer(-0.2, -0.2, 0.2, 0.2), Pull_Texture("Assets/Textures/Fire_2.png").Texture, Pull_Texture("Black").Texture, &Billboard_Fire_Particles);

	//

	Shader Billboard_Galaxy_Shader;
	Billboard_Galaxy_Shader.Create_Shader("Shader_Code/Billboard_Galaxy_Particle.vert", "Shader_Code/Lightless_Particle.frag", nullptr); // We really don't need a geometry shader for this one

	Create_Particle_Renderer(Billboard_Galaxy_Shader, Billboard_Vertex_Buffer(-0.2, -0.2, 0.2, 0.2), Pull_Texture("Assets/Textures/Galaxy_Test_2.png").Texture, Pull_Texture("Black").Texture, &Galaxy_Particles);

	/*for (float Radius = 0.3f; Radius < 10.0f; Radius += 0.15f)
	{
		for (size_t W = 0; W < 64 * Radius; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.3f, (W + RNG()) * 3.14159f * 2.0f / (64 * Radius));

		for (size_t W = 0; W < 256 * Radius; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.1f, 6.28318f + (W + RNG() * 4.0f) * 3.14159f * 2.0f / (64 * Radius));
	}*/
}

void Render_All()
{
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

	Cursor_Reset = true;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);						// Purely additive blending

	Scene_Object_Shader.Create_Shader("Shader_Code/Vertex_Test.vert", "Shader_Code/Vertex_Test.frag", "Shader_Code/Vertex_Test.geom");
	Scene_Object_Shader.Activate();

	//

	Load_Cubemap(
		{
		"Assets/Cubemap/Test/Cubemap_Left.png",
		"Assets/Cubemap/Test/Cubemap_Right.png",
		"Assets/Cubemap/Test/Cubemap_Top.png",
		"Assets/Cubemap/Test/Cubemap_Bottom.png",
		"Assets/Cubemap/Test/Cubemap_Back.png",
		"Assets/Cubemap/Test/Cubemap_Front.png"
		}, &Test_Cubemap);

	Push_Merged_Material("Assets/Textures/Brick_Specular.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Normal_Test.png", "Brick");

	Push_Merged_Material("Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Test_Normal.png", "Stone");

	Push_Merged_Material("Assets/Textures/Floor_Tile_Spec.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Floor_Tiles_Normal.png", "Floor");

	Push_Merged_Material("Assets/Textures/Brick_Specular.png", "Assets/Textures/Flat_Reflectivity.png", "Assets/Textures/Brick_Normal_Test.png", "Floor_Reflect");

	Push_Merged_Specular_Reflectivity("Assets/Textures/Black.png", "Assets/Textures/Black.png", "Black");

	Initialise_Particles();

	Initialise_UI_Shaders();

	UI_Elements.push_back(new UI_Element(-0.25, -0.1, 0.55, 0.5));

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
	Create_Model(Pull_Mesh("Assets/Models/Test_Level.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/White.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Level.obj").Mesh));

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

	Scene_Object_Shader.Activate();

	Model_Uniform_Location = Initialise_Model_Uniform_Locations_Object(Scene_Object_Shader);
	Light_Uniform_Location = Initialise_Light_Uniform_Locations_Object(Scene_Object_Shader);
	Camera_Uniform_Location = Initialise_Camera_Uniform_Locations_Object(Scene_Object_Shader);

	//

	Initialise_Job_System();

	if (Shadow_Mapper::Shadow_Mapping)
		Shadow_Mapper::Initialise_Shadow_Mapper();
}

void Engine_Loop()
{
	Setup_Test_Scene();

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
		Galaxy_Particles.Update();

		Scene_Object_Shader.Activate();

		Player_Camera.Set_Projection_Matrix();
		Player_Camera.Set_Audio_Observer();
		Player_Camera.Bind_Buffers(Camera_Uniform_Location);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);

		//

		// double Start_Timer = glfwGetTime();

		Render_All();

		glDisable(GL_DEPTH_TEST);

		Handle_UI(); // We're able to handle all of the UI whilst the collisions are resolving!

		Physics::Resolve_Collisions();

		Handle_Deletions();

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//

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
}

//

void Test_Shadow_Loop()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0u);

	Setup_Test_Scene();

	Last_Time = glfwGetTime();

	while (!glfwWindowShouldClose(Window))
	{
		double Current_Time = glfwGetTime();
		Tick = (Current_Time - Last_Time);
		Last_Time = Current_Time;

		Receive_Inputs();

		Player_Movement();

		glViewport(0, 0, 256, 256);

		glBindFramebuffer(GL_FRAMEBUFFER, Shadow_Mapper::Shadow_Frame_Buffer);

		glBindTexture(GL_TEXTURE_CUBE_MAP, Shadow_Mapper::Shadow_Maps[0].Shadow_Cubemap);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, Shadow_Mapper::Shadow_Maps[0].Shadow_Cubemap, 0u);

		glClearColor(0.3, 0.2, 0.2, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Update_Lighting_Buffer();

		Audio::Handle_Audio(Player_Camera);

		Shadow_Mapper::Shadow_Object_Shader.Activate();

		Scene_Object_Shader.Activate();

		Player_Camera.Set_Projection_Matrix();
		Player_Camera.Set_Audio_Observer();
		Player_Camera.Bind_Buffers(Camera_Uniform_Location);

		glUniformMatrix4fv(glGetUniformLocation(Scene_Object_Shader.Program_ID, "Projection_Matrix"), 1, GL_FALSE, &Shadow_Mapper::Shadow_Maps[0].View_Matrices[0][0][0]);

		// glUniformMatrix4fv(glGetUniformLocation(Shadow_Mapper::Shadow_Object_Shader.Program_ID, "Shadow_Matrix"), 1, GL_FALSE, glm::value_ptr(Projection_Matrix));

		for (size_t W = 0; W < Scene_Models.size(); W++)
			//Scene_Models[W]->Render(Shadow_Mapper::Shadow_Object_Shader);
			//Scene_Models[W]->Render(Scene_Object_Shader);// Shadow_Mapper::Shadow_Object_Shader);
			Shadow_Mapper::Render_Object_To_Shadow_Map(Scene_Models[W]);

		Handle_Deletions();

		Frame_Counter++;

		Time_Elapsed_Since_FPS_Update += Tick;

		if (Time_Elapsed_Since_FPS_Update > 1.0f)
		{
			glfwSetWindowTitle(Window, (std::string("VisionEngineGL | FPS: ") + std::to_string((int)(Frame_Counter / Time_Elapsed_Since_FPS_Update))).c_str());
			Time_Elapsed_Since_FPS_Update = 0;
			Frame_Counter = 0;
		}

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
}

#endif
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
#include "Particle_System_Declarations.h"

#include "Physics_Engine.h"

void Initialise_Particles()
{
	Shader Smoke_Particle_Shader;
	Smoke_Particle_Shader.Create_Shader("Shader_Code/Smoke_Particle.vert", "Shader_Code/Vertex_Test.frag", "Shader_Code/Vertex_Test.geom");

	Create_Particle_Renderer(Smoke_Particle_Shader, Pull_Mesh("Assets/Models/Particle_Test.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Transparent.png").Texture, Pull_Texture("Stone").Texture, &Smoke_Particles);
}

void Render_All()
{
	Test_Cubemap.Parse_Texture(Scene_Object_Shader, "Cubemap", 0);
	Test_Cubemap.Bind_Texture();

	Update_Lighting_Buffer();

	Scene_Lights[0]->Position = Player_Camera.Position;

	Scene_Lights[0]->Direction = Camera_Direction;

	for (size_t W = 0; W < Scene_Models.size(); W++)
	{
		Scene_Models[W]->Render(Scene_Object_Shader);
	}

	Smoke_Particles.Shader.Activate();

	glDepthMask(GL_FALSE);

	Test_Cubemap.Parse_Texture(Smoke_Particles.Shader, "Cubemap", 0);
	Test_Cubemap.Bind_Texture();

	Smoke_Particles.Render();
}

void Setup_Test_Scene()
{
	Scene_Lights.push_back(new Lightsource(glm::vec3(0, 3, -3), glm::vec3(1.5, 1, 1.3), glm::vec3(0, 0, 1), 60, 3));

	Cursor_Reset = true;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	Push_Merged_Material("Assets/Textures/Brick_Specular.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Test_Normal.png", "Stone");

	Push_Merged_Specular_Reflectivity("Assets/Textures/Black.png", "Assets/Textures/Black.png", "Black");

	Initialise_Particles();

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(0, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Viking_Room.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Viking_Room.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh));

	Scene_Models.push_back(new Model( { MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(0, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(5, -5, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh));

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

	Scene_Models.push_back(new Model());
	Scene_Models.back()->Position = glm::vec3(0, 5, -5);
	Create_Model(Pull_Mesh("Assets/Models/Ramp.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/White.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Controller(),  nullptr);

	Scene_Object_Shader.Activate();

	Model_Uniform_Location = Initialise_Model_Uniform_Locations_Object(Scene_Object_Shader);
	Light_Uniform_Location = Initialise_Light_Uniform_Locations_Object(Scene_Object_Shader);
	Camera_Uniform_Location = Initialise_Camera_Uniform_Locations_Object(Scene_Object_Shader);

	//

	Initialise_Job_System();
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

		glDepthMask(GL_TRUE);

		glClearColor(0.2, 0.3, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Scene_Lights[0]->Blur = 45 + 45 * sinf(glfwGetTime());

		Physics::Record_Collisions();
		
		Handle_Scene();

		Smoke_Particles.Update();

		Scene_Object_Shader.Activate();

		Player_Camera.Set_Projection_Matrix();
		Player_Camera.Bind_Buffers(Camera_Uniform_Location);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_CCW);
		glEnable(GL_DEPTH_TEST);

		//

		glEnable(GL_BLEND);

		Render_All();

		Physics::Resolve_Collisions();

		Handle_Deletions();

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
}

#endif
#ifndef ENGINE_LOOP_VISION
#define ENGINE_LOOP_VISION

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Uniform_Buffer_Declarations.h"
#include "Texture_Declarations.h"
#include "Asset_Loading_Cache.h"
#include "Input_Handler.h"
#include "Mesh_Loader.h"
#include "Model_Declarations.h"
#include "Lighting_Handler.h"

void Render_All()
{
	Update_Lighting_Buffer();

	Scene_Lights[0]->Position = Player_Camera.Position;

	for (size_t W = 0; W < Scene_Models.size(); W++)
	{
		Scene_Models[W]->Render(Scene_Object_Shader);
	}
}

void Setup_Test_Scene()
{
	Scene_Lights.push_back(new Lightsource(glm::vec3(0, 3, -3), glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), 50, 3));

	Cursor_Reset = true;

	Scene_Object_Shader.Create_Shader("Shader_Code/Vertex_Test.vert", "Shader_Code/Vertex_Test.frag");
	Scene_Object_Shader.Activate();

	//

	Push_Merged_Material("Assets/Textures/Brick_Specular.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Normal_Test.png", "Brick");

	Scene_Models.push_back(new Model());
	Scene_Models.back()->Position = glm::vec3(0, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Viking_Room.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Viking_Room.png").Texture, Pull_Texture("Assets/Textures/Black.png").Texture, Scene_Models.back());

	Scene_Models.push_back(new Model());
	Scene_Models.back()->Position = glm::vec3(0, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back());

	Scene_Models.push_back(new Model());
	Scene_Models.back()->Position = glm::vec3(9, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Assets/Textures/Black.png").Texture, Scene_Models.back());

	Initialise_Model_Uniform_Locations_Object(Scene_Object_Shader);
	Initialise_Light_Uniform_Locations_Object(Scene_Object_Shader);
}

void Engine_Loop()
{
	Setup_Test_Scene();

	Last_Time = glfwGetTime();

	while (!glfwWindowShouldClose(Window))
	{
		double Current_Time = glfwGetTime();
		Tick = Current_Time - Last_Time;
		Last_Time = Current_Time;

		Receive_Inputs();

		Player_Movement();

		//

		glClearColor(0.2, 0.3, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Scene_Models[0]->Orientation.x += Tick;

		Scene_Lights[0]->Blur = 45 + 45 * sinf(glfwGetTime());

		Render_All();

		Player_Camera.Set_Projection_Matrix();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_CCW);
		glEnable(GL_DEPTH_TEST);

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
}

#endif
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

void Render_All()
{
	for (size_t W = 0; W < Scene_Models.size(); W++)
	{
		Scene_Models[W]->Uniforms.Colour = glm::vec4(1, 1, 1, 1);
		Scene_Models[W]->Render();
	}
}

void Engine_Loop()
{
	Cursor_Reset = true;

	Model Test;
	Create_Model(Pull_Mesh("Assets/Models/Viking_Room.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Viking_Room.png").Texture, &Test);

	Shader Test_Shader;

	Test_Shader.Create_Shader("Shader_Code/Vertex_Test.vert", "Shader_Code/Vertex_Test.frag");
	Test_Shader.Activate();

	//

	Scene_Models.push_back(new Model());
	Scene_Models.back()->Position = glm::vec3(0, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Scene_Models.back());

	Initialise_Model_Uniform_Locations_Object(Test_Shader);

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

		/*Test.Bind_Buffer();

		Test_Uniform.Colour.x = 0.5 * (sin(glfwGetTime()) + 1);
		Test_Uniform.Colour.y = 0.5 * (cos(glfwGetTime()) + 1);
		Test_Uniform.Colour.z = 0.5 * (sin(glfwGetTime() * 2.0 + 0.7) + 1);

		Test_Uniform.Model_Matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.25 * sinf(glfwGetTime()), 0, -3));
		Test_Uniform.Model_Matrix = glm::rotate(Test_Uniform.Model_Matrix, (float)glfwGetTime() * 0.01f, glm::vec3(0, 0, 1));
		Test_Uniform.Model_Matrix = glm::rotate(Test_Uniform.Model_Matrix, (float)glfwGetTime() * 0.01f, glm::vec3(1, 0, 0));

		Test_Uniform.Update_Buffer();

		Test_Texture.Bind_Texture();*/

		Test.Uniforms.Colour.x = 0.5 * (sinf(glfwGetTime()) + 1);
		Test.Uniforms.Colour.y = 0.5 * (cosf(glfwGetTime()) + 1);
		Test.Uniforms.Colour.z = 0.5 * (sinf(glfwGetTime() * 2 + 0.7) + 1);

		Test.Position = glm::vec3(0.25 * sinf(glfwGetTime()), 0, -3);

		Test.Orientation.x += Tick;

		// Test.Orientation.y += 0.01f * Tick;
		// Test.Orientation.z += 0.01f * Tick;

		Test.Render();

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
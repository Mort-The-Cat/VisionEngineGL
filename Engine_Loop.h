#ifndef ENGINE_LOOP_VISION
#define ENGINE_LOOP_VISION

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Uniform_Buffer_Declarations.h"
#include "Texture_Declarations.h"
#include "Asset_Loading_Cache.h"
#include "Input_Handler.h"

void Engine_Loop()
{
	Cursor_Reset = true;

	Model_Vertex_Buffer Test({ Model_Vertex(0.5, 0.5, 1, 0, 0, 1, 1), Model_Vertex(0.5, -0.5, 0, 0, 1, 1, 0), Model_Vertex(-0.5, -0.5, 0, 1, 0, 0, 0), Model_Vertex(-0.5, 0.5, 1, 1, 1, 0, 1) }, { 0, 1, 3, 1, 2, 3 });
	Model_Uniform_Buffer Test_Uniform(glm::vec4(0.75, 0.5, 1, 1));

	Test.Create_Buffer();
	Test.Bind_Buffer();
	Test.Update_Buffer();

	//

	Texture Test_Texture;

	Get_Texture("Assets/Textures/Brick1.png", &Test_Texture);

	//

	Shader Test_Shader;

	Test_Shader.Create_Shader("Shader_Code/Vertex_Test.vert", "Shader_Code/Vertex_Test.frag");
	Test_Shader.Activate();

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

		Test.Bind_Buffer();

		Test_Uniform.Colour.x = 0.5 * (sin(glfwGetTime()) + 1);
		Test_Uniform.Colour.y = 0.5 * (cos(glfwGetTime()) + 1);
		Test_Uniform.Colour.z = 0.5 * (sin(glfwGetTime() * 2.0 + 0.7) + 1);

		Test_Uniform.Model_Matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.25 * sinf(glfwGetTime()), 0, -3));
		Test_Uniform.Model_Matrix = glm::rotate(Test_Uniform.Model_Matrix, (float)glfwGetTime(), glm::vec3(0, 0, 1));
		Test_Uniform.Model_Matrix = glm::rotate(Test_Uniform.Model_Matrix, (float)glfwGetTime(), glm::vec3(1, 0, 0));

		Test_Uniform.Update_Buffer();

		Test_Texture.Bind_Texture();

		Player_Camera.Set_Projection_Matrix();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, Test.Indices.size(), GL_UNSIGNED_INT, 0);

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
}

#endif
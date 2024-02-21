#ifndef ENGINE_LOOP_VISION
#define ENGINE_LOOP_VISION

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Input_Handler.h"

void Engine_Loop()
{
	Cursor_Reset = false;

	Model_Vertex_Buffer Test({ glm::vec2(0.5, 0.5), glm::vec2(0.5, -0.5), glm::vec2(-0.5, -0.5), glm::vec2(-0.5, 0.5) }, { 0, 1, 3, 1, 2, 3 });

	Test.Create_Buffer();
	Test.Bind_Buffer();
	Test.Update_Buffer();

	Shader Test_Shader;

	Test_Shader.Create_Shader("Shader_Code/Vertex_Test.vert", "Shader_Code/Vertex_Test.frag");
	Test_Shader.Activate();

	while (!glfwWindowShouldClose(Window))
	{
		Receive_Inputs();

		Player_Movement();

		//

		glClearColor(0.2, 0.3, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		Test.Bind_Buffer();

		glCullFace(GL_FALSE);

		glDrawElements(GL_TRIANGLES, Test.Indices.size(), GL_UNSIGNED_INT, 0);

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
}

#endif
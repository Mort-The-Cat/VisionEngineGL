#ifndef ENGINE_LOOP_VISION
#define ENGINE_LOOP_VISION

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Input_Handler.h"

void Engine_Loop()
{
	Cursor_Reset = false;

	Model_Vertex_Buffer Test({ glm::vec2(-0.5, -0.5), glm::vec2(0.5, -0.5), glm::vec2(0.0, 0.5) });

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

		glClearColor(1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
}

#endif

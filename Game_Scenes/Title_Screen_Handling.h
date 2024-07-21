#ifndef VISION_ENGINE_TITLE_SCREEN_HANDLING
#define VISION_ENGINE_TITLE_SCREEN_HANDLING

#include "../Engine_Loop_Includes.h"
#include "../Engine_Loop.h"

void Run_Engine_Loop(UI_Element* Element) 
{ 
	for (size_t W = 0; W < UI_Elements.size(); W++)
		UI_Elements[W]->Flags[UF_TO_BE_DELETED] = true;

	Galaxy_Particles.Delete_All_Particles();

	Setup_Test_Scene();

	Cursor_Reset = true;

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

	UI_Elements.push_back(new Button_Text_UI_Element(-1.75f, -0.9, 1.75f, -0.5f, Run_Engine_Loop, "Drücken Sie diesen Knopf, um anzufangen", 0.066667f, 0.1f));
	UI_Elements.back()->Image = Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture;

	UI_Elements.back()->Flags[UF_IMAGE] = true;

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

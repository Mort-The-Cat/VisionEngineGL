#ifndef ENGINE_LOOP_VISION
#define ENGINE_LOOP_VISION

#include "Engine_Loop_Includes.h"

void Render_All()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	glEnable(GL_LINEAR_MIPMAP_LINEAR);

	//

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

	glEnable(GL_DEPTH_CLAMP);
	Volumetric_Cone_Particles.Shader.Activate();
	Post_Processor::Bind_G_Buffer_Textures(Volumetric_Cone_Particles.Shader);
	Bind_Screen_Dimensions(Volumetric_Cone_Particles.Shader);
	Volumetric_Cone_Particles.Render();
	glDisable(GL_DEPTH_CLAMP);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We'll be using purely additive blending for the fire particles
	Billboard_Fire_Particles.Shader.Activate();
	Billboard_Fire_Particles.Render();

	//

	// Galaxy_Particles.Shader.Activate();
	// Galaxy_Particles.Render();
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
		Volumetric_Cone_Particles.Update();
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
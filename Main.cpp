#include <stdio.h>
#include <iostream>

#include "OpenGL_Declarations.h"
#include "Engine_Loop.h"
#include "Audio_Declarations.h"

#include "Post_Processor_Declarations.h"
#include "Shadow_Map_Renderer_Declarations.h"

#include "Job_System_Declarations.h"

#include "UI_Renderer_Declarations.h"

#include "Game_Scenes/Title_Screen_Handling.h"

#include "Game_Scenes/Shader_Etc_Initialisation.h"

int main()
{
	printf(" >> Hello!\n");

	Initialise_OpenGL_Window();

	if(Post_Processing)
		Post_Processor::Initialise_Post_Processor();

	// Font_Table::Initialise_Font_Texture();

	Initialise_Sound_Engine();

	Initialise_Shaders_And_Particle_Emitters();
	
	Create_Title_Screen_Page();

	//Engine_Loop();

	// Test_Shadow_Loop();

	Close_Program();

	Job_System::Working = false; // This will tell the worker threads that they can relax now

	Destroy_Sound_Engine();

	std::cin.get();

	return 0;
}
#include <stdio.h>
#include <iostream>

#include "OpenGL_Declarations.h"
#include "Engine_Loop.h"
#include "Audio_Declarations.h"

#include "Post_Processor_Declarations.h"
#include "Shadow_Map_Renderer_Declarations.h"
int main()
{
	printf(" >> Hello!\n");

	Initialise_OpenGL_Window();

	if(Post_Processing)
		Post_Processor::Initialise_Post_Processor();

	if (Shadow_Mapper::Shadow_Mapping)
		Shadow_Mapper::Initialise_Shadow_Mapper();

	Initialise_Sound_Engine();
	Engine_Loop();
	Close_Program();

	Destroy_Sound_Engine();

	std::cin.get();

	return 0;
}
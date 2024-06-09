#include <stdio.h>
#include <iostream>

#include "OpenGL_Declarations.h"
#include "Engine_Loop.h"
#include "Audio_Declarations.h"

#include "Post_Processor_Declarations.h"

int main()
{
	printf(" >> Hello!\n");

	Initialise_OpenGL_Window();

	Post_Processor::Initialise_Post_Processor();

	Initialise_Sound_Engine();
	Engine_Loop();
	Close_Program();

	Destroy_Sound_Engine();

	std::cin.get();

	return 0;
}
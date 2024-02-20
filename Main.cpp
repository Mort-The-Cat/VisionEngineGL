#include <stdio.h>
#include <iostream>

#include "OpenGL_Declarations.h"
#include "Engine_Loop.h"

int main()
{
	printf(" >> Hello!\n");

	Initialise_OpenGL_Window();
	Engine_Loop();
	Close_Program();

	std::cin.get();

	return 0;
}
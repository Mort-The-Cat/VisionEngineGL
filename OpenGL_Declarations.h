#ifndef OPENGL_DECLARATIONS_VISION
#define OPENGL_DECLARATIONS_VISION

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"

#include<array>
#include<vector>
#include<istream>
#include<fstream>
#include<iostream>

void Throw_Error(const char* Error_Message)
{
	printf("%s", Error_Message);
	std::cin.get();
}

GLFWwindow* Window = nullptr;
int Window_Width = 1600, Window_Height = 800;

std::string Get_File_Contents(const char* Directory)
{
	std::ifstream File(Directory);
	if (File)
	{
		std::string Contents;
		File.seekg(0, std::ios::end);
		Contents.resize(File.tellg());
		File.seekg(0, std::ios::beg);

		File.read(&Contents[0], Contents.size());

		File.close();

		return Contents;
	}
	Throw_Error(" >> Unable to load file!");
}

class Shader
{
private:
	void Check_Shader_Errors(unsigned int Shader, const char* Type, const char* Name)
	{
		GLint Success;
		char Info_Log[1024];
		if (Type != "PROGRAM")
		{
			// We're checking either the vertex or fragment shader

			glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);

			if (Success == GL_FALSE)
			{
				glGetShaderInfoLog(Shader, 1024, NULL, Info_Log);
				printf("\n >> SHADER COMPILATION ERROR! %s\n%s\n%s\n", Type, Info_Log, Name);
			}
		}
		else
		{
			// We know we're checking the *whole* shader program

			glGetProgramiv(Shader, GL_LINK_STATUS, &Success);

			if (Success == GL_FALSE)
			{
				glGetProgramInfoLog(Shader, 1024, NULL, Info_Log);
				printf("\n >> SHADER COMPILATION ERROR! %s\n%s\n%s\n", Type, Info_Log, Name);
			}
		}
	}
public:
	unsigned int Shader_Program;

	void Activate()
	{
		glUseProgram(Shader_Program);
	}

	void Create_Shader(const char* Vertex_File, const char* Fragment_File)
	{
		unsigned int Vertex_Shader, Fragment_Shader;

		Vertex_Shader = glCreateShader(GL_VERTEX_SHADER);
		Fragment_Shader = glCreateShader(GL_FRAGMENT_SHADER);

		std::string Vertex_Code = Get_File_Contents(Vertex_File);
		std::string Fragment_Code = Get_File_Contents(Fragment_File);
		const char* Vertex_Source = Vertex_Code.c_str();
		const char* Fragment_Source = Fragment_Code.c_str();

		glShaderSource(Vertex_Shader, 1, &Vertex_Source, NULL);
		glCompileShader(Vertex_Shader);

		Check_Shader_Errors(Vertex_Shader, "VERTEX", Vertex_File);

		glShaderSource(Fragment_Shader, 1, &Fragment_Source, NULL);
		glCompileShader(Fragment_Shader);

		Check_Shader_Errors(Fragment_Shader, "FRAGMENT", Fragment_File);

		Shader_Program = glCreateProgram();

		glAttachShader(Shader_Program, Vertex_Shader);
		glAttachShader(Shader_Program, Fragment_Shader);
		glLinkProgram(Shader_Program);

		Check_Shader_Errors(Shader_Program, "PROGRAM", "");

		glDeleteShader(Vertex_Shader);
		glDeleteShader(Fragment_Shader);
	}
	
};

void Framebuffer_Resize_Callback(GLFWwindow* Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
}

void Initialise_OpenGL_Window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	Window = glfwCreateWindow(Window_Width, Window_Height, "Vision engine test!", NULL, NULL);

	if (Window == NULL)
		Throw_Error(" >> Failed to create OpenGL Window!\n");

	glfwMakeContextCurrent(Window);

	gladLoadGL();


	//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	//	Throw_Error(" >> Failed to initialise glad!\n");

	glfwSetWindowSizeCallback(Window, Framebuffer_Resize_Callback);
}

void Close_Program()
{
	glfwTerminate();
}

#endif

#ifndef INPUT_HANDLER_VISION
#define INPUT_HANDLER_VISION

// In this, we'll really want a system that allows the player to re-map inputs etc

// We may have to refactor this in order to get the linker to behave

#include "OpenGL_Declarations.h"
#include "Job_System.h"
#include "Asset_Loading_Cache.h"
#include "Physics_Engine.h"

#include "Audio_Declarations.h"
#include "Audio_Handler_Declarations.h"

#include "Particle_System_Declarations.h"

bool Inputs[11];

std::array<uint16_t, 11> Inputs_Keycode = // The values of these keycodes can be changed during runtime if the user wishes to adjust the controls for whatever reason
{
	GLFW_KEY_W, // Forwards
	GLFW_KEY_S, // backwards
	GLFW_KEY_A, // left
	GLFW_KEY_D, // right

	GLFW_KEY_SPACE,			// up 
	GLFW_KEY_LEFT_SHIFT,	// down

	GLFW_KEY_ESCAPE,		// Pause

	GLFW_KEY_Q,				// Lean left
	GLFW_KEY_E,				// Lean right

	GLFW_KEY_F, // Use

	GLFW_KEY_R // Reload / auxilliary
};

float Mouse_Sensitivity = 0.5;

bool Cursor_Reset = true;

glm::vec2 Cursor;

bool Mouse_Inputs[2]; // Left and right respectively

namespace Controls // These are the indices for every user input. This may need some extra work if the user wishes to *type* something into an in-engine text box.
{
	uint8_t Forwards = 0;
	uint8_t Backwards = 1;
	uint8_t Left = 2;
	uint8_t Right = 3;

	uint8_t Up = 4;
	uint8_t Down = 5;

	uint8_t Pause = 6;

	uint8_t Lean_Left = 7;
	uint8_t Lean_Right = 8;

	uint8_t Use = 9;
	uint8_t Auxilliary = 10;
};
//



void Receive_Inputs() // This sets all of the bits in "inputs", ready to be processed
{
	for (uint8_t W = 0; W < Inputs_Keycode.size(); W++)
		Inputs[W] = glfwGetKey(Window, Inputs_Keycode[W]) == GLFW_PRESS; // The inputs can be handled later

	Mouse_Inputs[0] = glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;		// The mouse buttons can't be remapped in-engine, but they can be used for whatever you want
	Mouse_Inputs[1] = glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	double X, Y;

	glfwGetCursorPos(Window, &X, &Y);

	if (Cursor_Reset) glfwSetCursorPos(Window, Window_Width >> 1, Window_Height >> 1); // Middle of the screen

	Cursor = { X - (Window_Width >> 1), Y - (Window_Height >> 1) }; // This gets the cursor position! Very simple ^^

	Cursor *= glm::vec2(Mouse_Sensitivity / Window_Width, -Mouse_Sensitivity / Window_Height);
}

void Close_Game()
{
	glfwSetWindowShouldClose(Window, 1);
}

float Total_Timer = 0;

uint32_t Frames = 0;

void Spawn_Test_Object()
{
	for (size_t W = 0; W < 1; W++)
	{
		Scene_Models.push_back(new Model({ MF_ACTIVE, MF_PHYSICS_TEST, MF_SOLID }));
		Scene_Models.back()->Position = Player_Camera.Position + glm::vec3(RNG() * 1 - .5, RNG() * 1 - .5, RNG() * 1 - .5);
		Create_Model(Pull_Mesh("Assets/Models/Particle_Test.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/White.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Physics_Object_Controller(), Generate_Sphere_Hitbox(*Pull_Mesh("Assets/Models/Particle_Test.obj").Mesh));
		//Create_Model(Pull_Mesh("Assets/Models/Mesh_Hitbox.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/White.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Physics_Object_Controller(), Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Models/Mesh_Hitbox.obj").Mesh));
	}
}

void Shoot_Fire(float Angle)
{
	glm::vec3 Raycast_Velocity(-sin(Angle) * cos(DTR * Player_Camera.Orientation.y), -sin(DTR * Player_Camera.Orientation.y), -cos(Angle) * cos(DTR * Player_Camera.Orientation.y));

	Hitbox* Target = nullptr;
	Collision_Info Info = Collision_Test::Raycast(Player_Camera.Position, Raycast_Velocity * glm::vec3(0.01), 500, Collision_Test::Always_Compare, &Target);

	if (Target != nullptr)
	{
		Fire_Sound->setVolume(1);

		Scene_Lights.push_back(new Lightsource(Info.Collision_Position - Info.Collision_Normal * glm::vec3(0.1), glm::vec3(RNG() * 1 + 2, RNG() + 1, RNG()), Info.Collision_Normal, 360, 1));
		Scene_Lights.back()->Flags[LF_TO_BE_DELETED] = true;

		for (size_t W = 0; W < 1; W++)
			Billboard_Smoke_Particles.Particles.Spawn_Particle(Info.Collision_Position + glm::vec3(0.1 * RNG() - 0.05, 0.1 * RNG() - 0.05, 0.1 * RNG() - 0.05), glm::vec3(-2) * Info.Collision_Normal + glm::vec3(.5 * RNG() - 0.25, .5 * RNG() - 0.25, .5 * RNG() - 0.25));

		if (Target->Object->Flags[MF_PHYSICS_TEST]) // If the object is a physics object
		{
			Physics_Object_Controller* Control = (Physics_Object_Controller*)Target->Object->Control;

			Control->Time = -1;

			Sound_Engine->play2D(Sound_Effect_Source);
		}
	}
}

void Player_Movement()
{
	if (Inputs[Controls::Pause])
	{
		Close_Game();
	}

	if (Inputs[Controls::Use])
	{
		Spawn_Test_Object();
	}

	float Speed = -2.5 * Tick;

	float Angle = -DTR * Player_Camera.Orientation.x;

	float Movement_X = sin(Angle) * Speed;
	float Movement_Z = cos(Angle) * Speed;

	//if (Inputs[Controls::Auxilliary] && Frame_Counter % 10 == 0)
	//{
		//Audio::Audio_Sources.back()->Play_Sound(Sound_Effect_Source);

		//Billboard_Smoke_Particles.Particles.Spawn_Particle(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	//}

	if (Mouse_Inputs[0]) // If left-click,
	{
		// we wanna apply a force onto some objects!

		Shoot_Fire(Angle);
	}
	else
		Fire_Sound->setVolume(0);

	if (Inputs[Controls::Forwards])
	{
		Player_Camera.Position.x += Movement_X;
		Player_Camera.Position.z += Movement_Z;
	}
	if (Inputs[Controls::Backwards])
	{
		Player_Camera.Position.x -= Movement_X;
		Player_Camera.Position.z -= Movement_Z;
	}
	if (Inputs[Controls::Left])
	{
		Player_Camera.Position.x += Movement_Z;
		Player_Camera.Position.z -= Movement_X;
	}
	if (Inputs[Controls::Right])
	{
		Player_Camera.Position.x -= Movement_Z;
		Player_Camera.Position.z += Movement_X;
	}

	if (Inputs[Controls::Lean_Left])
		Player_Camera.Orientation.z += Tick * 90;
	if (Inputs[Controls::Lean_Right])
		Player_Camera.Orientation.z -= Tick * 90;

	if (Inputs[Controls::Down])
		Player_Camera.Position.y -= Speed;
	if (Inputs[Controls::Up])
		Player_Camera.Position.y += Speed;

	Player_Camera.Orientation.x += Cursor.x * 90;
	Player_Camera.Orientation.y += Cursor.y * 90;

	Player_Camera.Orientation.y = std::max(Player_Camera.Orientation.y, -90.0f);
	Player_Camera.Orientation.y = std::min(Player_Camera.Orientation.y, 90.0f);

	//if (Inputs[Controls::Auxilliary])
	//{
	//	printf(" >> FPS: %f\n", 1.0f / Tick);
	//}
}

#endif
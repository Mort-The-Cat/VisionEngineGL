#ifndef VISION_UI_TEXTBOX_DEFINITIONS
#define VISION_UI_TEXTBOX_DEFINITIONS

#include "UI_Renderer_Declarations.h"
#include "Input_Handler.h"

namespace UI_Typing
{
#define NUMBER_OF_KEYS 270u

	bool Previous_Keys[NUMBER_OF_KEYS];
	bool Current_Keys[NUMBER_OF_KEYS];

	bool Key_Pressed(size_t Key)
	{
		return Current_Keys[Key] && !Previous_Keys[Key];
	}

	void Update_Keys()
	{
		memcpy(Previous_Keys, Current_Keys, sizeof(Current_Keys));

		for (size_t W = 0; W < NUMBER_OF_KEYS; W++)
			Current_Keys[W] = glfwGetKey(Window, W);
	}

	bool Handle_Writing_Character_Inputs(std::string& Text, size_t& Cursor_Position)
	{
		Update_Keys();

		bool Update = false;

		if (Key_Pressed(GLFW_KEY_BACKSPACE) && Cursor_Position > 0u)
		{
			Update = true;

			Text.erase(Cursor_Position - 1, 1u);

			Cursor_Position--;
		}

		if (Key_Pressed(GLFW_KEY_LEFT) && Cursor_Position)
			Cursor_Position--;
		else if (Key_Pressed(GLFW_KEY_RIGHT) && Cursor_Position < Text.length())
			Cursor_Position++;

		for (size_t W = 0; W < 95; W++)
			if (Key_Pressed(W))
			{
				Update = true;
				
				char Letters[2] = " ";

				Letters[0] = W;

				Text.insert(Cursor_Position, Letters);

				Cursor_Position++;
			}

		return Update;
	}
}

class Textbox_UI_Element : public Text_UI_Element // This is a textbox which the user can type in
{
public:
	std::string Text = ""; // This is the text that the user has written so far. Explicitly separate from "character_indices"

	bool Selected_For_Writing_In = false; // This flag is set when the user begins writing in this UI_Element- either being forced to or by clicking on it

	size_t Cursor_Position = 0u; // This is the position of the cursor in the text

	// Remember to manually unset this flag once the user has stopped typing in here - check if the user clicks off this UI element

	Textbox_UI_Element(float X1p, float Y1p, float X2p, float Y2p, float Sizep = 1.0f / 15.0f, float Italic_Slantp = 0.0f)
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;

		Size = Sizep;
		Italic_Slant = Italic_Slantp;
	}

	void Render_Cursor(UI_Transformed_Coordinates Coords)
	{
		// The hardest part about this is probably deciding where the cursor is to be rendered

		size_t Line_Length = static_cast<size_t>((Coords.X2o - Coords.X1o - Size * 2 * Window_Aspect_Ratio) / ((Size + 0.01f) * Window_Aspect_Ratio));

		float Width_Of_Letter = (Size + 0.01f) * Window_Aspect_Ratio;

		float Height_Of_Letter = (Size) * (0.9f + Font_Table::Character_Aspect_Ratio);

		size_t Horizontal_Offset = Cursor_Position % Line_Length;
		size_t Vertical_Offset = Cursor_Position / Line_Length;

		float Y_Offset = (Vertical_Offset - 0.1f) * Height_Of_Letter;
		float X_Offset = (Horizontal_Offset - 0.1f) * Width_Of_Letter;

		Billboard_Vertex_Buffer Letter(
			Coords.X1o + Size * Window_Aspect_Ratio + X_Offset,
			Coords.Y2o - Size - Y_Offset,
			Coords.X1o + Size * 2 * Window_Aspect_Ratio + X_Offset,
			Coords.Y2o - Size * (1 + Font_Table::Character_Aspect_Ratio) - Y_Offset,

			glm::vec2(0, 1.0f), glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),

			Italic_Slant * Window_Aspect_Ratio
		);

		glDrawElements(GL_TRIANGLES, Letter.Indices_Count, GL_UNSIGNED_INT, 0u);

		Letter.Delete_Buffer();
	}

	//

	virtual void Update_UI() override
	{
		UI_Transformed_Coordinates Coords(X1, Y1, X2, Y2, UI_Border_Size, Flags[UF_CLAMP_TO_SIDE], Flags[UF_FILL_SCREEN]);

		bool Hovering = Button_Hover(Coords);

		Colour = glm::vec4(1, 1, 1, 1.0f);

		if (Hovering)
			Colour.b = 0.75f;
		else
			Colour.b = 1.0f;

		if (Hovering && Mouse_Inputs[0])
			Colour *= glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);

		Render(Coords);

		/*if (Mouse_Unclick(0))	// If the mouse just unclicked
		{
			Selected_For_Writing_In = Hovering;	// We set the writing flag to whether or not the user is hovering over this element when the user clicked.

			Text += (char)(Cursor_Position + 20u);

			Cursor_Position++;

			Character_Indices.clear();

			Font_Table::Generate_Text_Indices(Text.c_str(), &Character_Indices);
		}*/

		if (Mouse_Unclick(0))
			Selected_For_Writing_In = Hovering;

		if (Selected_For_Writing_In)
		{
			bool Update = UI_Typing::Handle_Writing_Character_Inputs(Text, Cursor_Position);

			if (Update)
			{
				Character_Indices.clear();

				Font_Table::Generate_Text_Indices(Text.c_str(), &Character_Indices);
			}
		}
	}

	//

	virtual void Render(UI_Transformed_Coordinates Coords) override
	{
		UI_Shader.Activate();

		if (Flags[UF_RENDER_BORDER])
			Render_Border(Coords);

		Render_Screen_Sprite(Coords.X1o, Coords.Y1o, Coords.X2o, Coords.Y2o,
			{ 0, 1 }, { 1, 1 }, { 0, 0 }, { 1, 0 });

		Bind_UI_Uniforms(UI_Shader, Pull_Texture("Assets/Font/Cursor.png").Texture, Colour);

		if (Selected_For_Writing_In)
			Render_Cursor(Coords);

		Text_Shader.Activate();

		Bind_UI_Uniforms(Text_Shader, Font_Table::Font, Colour);

		Render_Text(Coords);
	}
};

#endif
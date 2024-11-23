#ifndef VISION_ENGINE_UI_RENDERER_DECLARATIONS
#define VISION_ENGINE_UI_RENDERER_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Texture_Declarations.h"
#include "Asset_Loading_Cache.h"

#define USING_FREETYPE_FONT 1u

#ifdef USING_FREETYPE_FONT

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Font_Table
{
	struct Character
	{
		Texture Glyph;
		glm::ivec2 Size; // These are the dimensions of the glyph in pixels
		glm::ivec2 Offset; // These are the bearings of the glyph in pixels; i.e. the location of the top-left corner of the glyph
		unsigned int Step; // This is how far along (in pixels) the next character should be placed
	};

	// std::map<char, Character> Characters;

	// const float Character_Pixel_To_Screen_Space = 1.0f / 512;

	class Font
	{
	public:
		std::map<char, Character> Characters;

		float Character_Pixel_To_Screen_Space;
	};

	void Initialise_Font_Texture(Font* Target_Font, size_t Character_Resolution, std::string File_Name)
	{
		FT_Library Interface;

		if (FT_Init_FreeType(&Interface))
			Throw_Error(" >> Unable to initialise freetype interface!\n");

		FT_Face Face;

		if (FT_New_Face(Interface, File_Name.c_str(), 0, &Face))
			Throw_Error(" >> Unable to load font!\n");

		FT_Set_Pixel_Sizes(Face, 0, Character_Resolution);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1u);

		for (int Char = 32; Char < 256; Char++)
		{
			int Glyph_Index = FT_Get_Char_Index(Face, Char);

			if (FT_Load_Glyph(Face, Glyph_Index, FT_LOAD_RENDER))
				Throw_Error(" >> Unable to load font glyph!\n");

			//if (FT_Load_Char(Face, Char, FT_LOAD_RENDER))
			//	Throw_Error(" >> Unable to load font glyph!\n");

			if (Face->glyph->bitmap.width)
			{
				Character Letter;

				Letter.Glyph.Create_Texture();

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Face->glyph->bitmap.width, Face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, Face->glyph->bitmap.buffer);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

				glGenerateMipmap(GL_TEXTURE_2D);

				Letter.Size = glm::ivec2(Face->glyph->bitmap.width, Face->glyph->bitmap.rows);
				Letter.Offset = glm::ivec2(Face->glyph->bitmap_left, Face->glyph->bitmap_top);
				Letter.Step = Face->glyph->advance.x >> 6;

				Target_Font->Characters.insert(std::pair<char, Character>(Char, Letter));
			}
		}

		Target_Font->Character_Pixel_To_Screen_Space = 1.0f / (float)Character_Resolution;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4u); // This sets the unpack alignment to default values

		FT_Done_Face(Face);
		FT_Done_FreeType(Interface);

		// This frees the faces and interface we use to generate the glyph textures
	}
}

Font_Table::Font Font_Georgia;
Font_Table::Font Font_Gothic;

#elif

namespace Font_Table
{
	uint8_t Table[] = { 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 69, 0, 0, 63, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 69, 0, 0, 63, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 77, 76, 0, 0, 0, 0, 0, 70, 71, 0, 0, 75, 0, 74, 0, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 72, 73, 0, 0, 0, 78, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };

	uint8_t Get_Font_Index(char Input)
	{
		return Table[Input + 124];
	}

	void Generate_Text_Indices(const char* Input_String, std::vector<uint32_t>* Indices)
	{
		size_t W = 0;
		
		while(Input_String[W] != '\0')
		{
			if (Input_String[W] == -61) // If it's a non-important character, 
				W++;							// we just skip it

			Indices->push_back(Get_Font_Index(Input_String[W]));

			W++;
		}
	}

	// abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ﬂ‰ˆ¸ƒ÷‹():;.,"!?

	// Here is the specific order of the font characters used in this table

	Texture Font; // This is the texture for the font we're using. Every character has the same height and width so getting each character is analogous to array-indexing.

	const size_t Character_Width = 170u; // We will have a *slightly* larger letter image to prevent letters bleeding over across elements in the texture atlas
	const size_t Character_Height = 495u;

	const size_t Font_Grid_Width = 16u;
	const size_t Font_Grid_Height = 5u;

	const size_t Image_Size = Font_Grid_Width * Font_Grid_Height * (Character_Width * Character_Height * 4u);

	constexpr float Character_Aspect_Ratio = static_cast<float>(Character_Height) / static_cast<float>(Character_Width);

	void Create_Font_Texture(std::vector<std::string> Directories)
	{
		Font.Create_Texture();

		stbi_uc* Pixels = new stbi_uc[Image_Size];

		memset(Pixels, 0u, Image_Size);

		for (size_t W = 0; W < Directories.size(); W++)
		{
			std::string Directory = "Assets/Font/" + Directories[W] + ".png";

			int Width, Height, Components;

			stbi_uc* Graphic = stbi_load(Directory.c_str(), &Width, &Height, &Components, 0u);

			size_t X_Offset = W % Font_Grid_Width;
			size_t Y_Offset = W / Font_Grid_Width;

			//memcpy(&Pixels[Character_Width * Character_Height * 4u * W], Graphic, Character_Width * Character_Height * 4u);

			for (size_t Y = 0; Y < Height; Y++)
				memcpy(&Pixels[(Font_Grid_Width * (Y + Y_Offset * Character_Height) + X_Offset) * Character_Width * 4u], &Graphic[Y * Width * 4u], Width * 4u);

			stbi_image_free(Graphic);
		}

		glTexImage2D(GL_TEXTURE_2D, 0u, GL_RGBA, Character_Width * Font_Grid_Width, Character_Height * Font_Grid_Height, 0u, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(Pixels);
	}

	void Initialise_Font_Texture()
	{
		Create_Font_Texture(
			{
				"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",

				"Capital_A", "Capital_B", "Capital_C", "Capital_D", "Capital_E", "Capital_F", "Capital_G", "Capital_H", "Capital_I", "Capital_J", "Capital_K",
				"Capital_L", "Capital_M", "Capital_N", "Capital_O", "Capital_P", "Capital_Q", "Capital_R", "Capital_S", "Capital_T", "Capital_U", "Capital_V",
				"Capital_W", "Capital_X", "Capital_Y", "Capital_Z",

				"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
				"Space",
				"Eszett", "‰", "ˆ", "¸", "Capital_ƒ", "Capital_÷", "Capital_‹",

				"Open_Bracket", "Close_Bracket", "Colon", "Semicolon", "Fullstop", "Comma", "Quote", "Exclamation", "Question"
			});
	}
}

#endif

#define UF_TO_BE_DELETED 0u
#define UF_RENDER_BORDER 1u
#define UF_IMAGE 2u				// This flag is set if the UI element has an image that is to be displayed- otherwise, it just uses the UI texture it was using before
#define UF_CLAMP_TO_SIDE 3u		// This flag is set if you want the UI element to stay at either side of the screen (useful to account for widescreen aspect ratios)
#define UF_FILL_SCREEN 4u		// This flag is set for things like backgrounds- useful for title screen
#define UF_RENDER_CONTENTS 5u	// This flag is set if you only want the UI border to be rendered
#define UF_CLAMP_RIGHT 6u		// This flag toggles which side of the screen the UI is clamped to

bool UI_Continue_Looping = false;

Shader UI_Shader;
Shader Text_Shader;

#ifndef USING_FREETYPE_FONT

void Initialise_UI_Shaders()
{
	UI_Shader.Create_Shader("Shader_Code/UI_Shader.vert", "Shader_Code/UI_Shader.frag", nullptr);

	Text_Shader.Create_Shader("Shader_Code/Text_Renderer_Old.vert", "Shader_Code/Text_Renderer_Old.frag", nullptr);
}

#else

void Initialise_UI_Shaders()
{
	UI_Shader.Create_Shader("Shader_Code/UI_Shader.vert", "Shader_Code/UI_Shader.frag", nullptr);
	Text_Shader.Create_Shader("Shader_Code/Text_Renderer.vert", "Shader_Code/Text_Renderer.frag", nullptr);
}

#endif

void Bind_UI_Uniforms(Shader UI_Shader, Texture Image, glm::vec4 Colour)
{
	glUniform4f(glGetUniformLocation(UI_Shader.Program_ID, "Colour"), Colour.x, Colour.y, Colour.z, Colour.w);

	//Font_Table::Font.Parse_Texture(UI_Shader, "Albedo", 0u);
	//Font_Table::Font.Bind_Texture();

	Image.Parse_Texture(UI_Shader, "Albedo", 0u);
	Image.Bind_Texture();
}

void Render_Screen_Sprite(float X1, float Y1, float X2, float Y2, glm::vec2 TL_UV, glm::vec2 TR_UV, glm::vec2 BL_UV, glm::vec2 BR_UV)
{
	// Make vertex buffer for this

	Billboard_Vertex_Buffer Vertex_Buffer(X1, Y1, X2, Y2, TL_UV, TR_UV, BL_UV, BR_UV); // This draws the image within the UI border

	// Vertex_Buffer.Bind_Buffer();

	glDrawElements(GL_TRIANGLES, Vertex_Buffer.Indices_Count, GL_UNSIGNED_INT, 0u);

	//

	Vertex_Buffer.Delete_Buffer(); // After we use it, we can just delete it

	// Might optimise this later but I don't care that much for the UI
}

/*

[-1, -1]-------[1,-1]
	|			|
	|			|
	|			|
	|			|
[-1, 1]--------[1, 1]


// This is how the coordinates for the UI are layed out.

X1,Y1 is equal to the top-left coords

X2,Y2 is equal to the bottom-right coords

*/

// From there, we will also handle aspect-ratio issues

// We want the vertical axis to remain between -1 and 1. The horizontal axis can change accordingly

class UI_Transformed_Coordinates
{
	void Shift_Right(float* Left_X, float* Right_X)
	{
		float Delta = std::min(*Left_X + 1.0f, 0.0f);

		*Left_X -= Delta;
		*Right_X -= Delta;
	}

	void Shift_Left(float* Left_X, float* Right_X)
	{
		float Delta = std::max(0.0f, *Right_X - 1.0f);

		*Left_X -= Delta;
		*Right_X -= Delta;
	}

public:
	float X1, Y1, X2, Y2; // Bounds

	float X1o, Y1o, X2o, Y2o; // Offset
	UI_Transformed_Coordinates(float X1p, float Y1p, float X2p, float Y2p, float UI_Border_Size, bool Clamp_To_Side, bool Fill_Screen = false, bool Clamp_Right = false)
	{
		if (Clamp_To_Side) // This preserves the horizontal dimensions of the UI element
		{
			float Width = X2p - X1p;

			// if ((X1p + X2p) > 0.0f)
			if(Clamp_Right)
			{
				Shift_Left(&X1p, &X2p);

				X2p = (1.0f / Window_Aspect_Ratio) + (1.0f + X2p);
				X1p = X2p - Width;
			}
			else
			{
				Shift_Right(&X1p, &X2p);

				X1p = (X1p + 1.0f) - (1.0f / Window_Aspect_Ratio);
				X2p = X1p + Width;

			}
		}

		if (Fill_Screen)
		{
			X1p /= Window_Aspect_Ratio;
			X2p /= Window_Aspect_Ratio;
		}

		X1 = X1p;
		X1o = X1p + UI_Border_Size;

		Y1 = -Y2p;
		Y1o = UI_Border_Size - Y2p;

		X2 = X2p;
		X2o = X2p - UI_Border_Size;

		Y2 = -Y1p;
		Y2o = -UI_Border_Size - Y1p;

		//

		X1 *= Window_Aspect_Ratio;
		X1o *= Window_Aspect_Ratio;
		X2 *= Window_Aspect_Ratio;
		X2o *= Window_Aspect_Ratio;
	}
};

class UI_Element // The subclasses hereof will handle things like text, buttons, etc
{
public:
	float X1, Y1, X2, Y2;

	bool Flags[7] = { false, true, false, false, false, true, false };

	float UI_Border_Size = 1.0f / 20.0f;

	float Inv_UI_Border_Size = 20.0f;

	glm::vec4 Colour = glm::vec4(1, 1, 1, 1.0f);

	Texture Image;

	// X1 and Y1 are the top-left corner of the screen

	// X2 and Y2 are the bottom-right corner of the screen

	UI_Element() {}

	UI_Element(float X1p, float Y1p, float X2p, float Y2p, Texture Imagep = Texture())
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;

		Image = Imagep;

		Flags[UF_IMAGE] = Image.Texture_Created();
	}

	bool Button_Hover(UI_Transformed_Coordinates Coords)
	{
		return Coords.X1 <= Cursor.x && Coords.X2 >= Cursor.x && Coords.Y1 <= Cursor.y && Coords.Y2 >= Cursor.y;
	}

	virtual void Render_Border(UI_Transformed_Coordinates Coords)
	{
		Bind_UI_Uniforms(UI_Shader, Pull_Texture("Assets/UI/UI_Corner.png").Texture, Colour);

		Render_Screen_Sprite(Coords.X1, Coords.Y1, Coords.X1o, Coords.Y1o,
			{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 });

		Render_Screen_Sprite(Coords.X2o, Coords.Y1, 
			Coords.X2, Coords.Y1o,
			{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 });

		Render_Screen_Sprite(Coords.X1, Coords.Y2o, 
			Coords.X1o, Coords.Y2,
			{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 });

		Render_Screen_Sprite(Coords.X2o, Coords.Y2o, 
			Coords.X2, Coords.Y2,
			{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 });

		//

		Bind_UI_Uniforms(UI_Shader, Pull_Texture("Assets/UI/UI_Line.png").Texture, Colour);

		float Delta_X_UV = (Coords.X2 - Coords.X1) * Inv_UI_Border_Size - 2;
		float Delta_Y_UV = (Coords.Y2 - Coords.Y1) * Inv_UI_Border_Size - 2;

		Render_Screen_Sprite(Coords.X1o, Coords.Y1,
			Coords.X2o, Coords.Y1o, 
			{ 0, 0 }, { Delta_X_UV, 0 }, { 0, 1 }, { Delta_X_UV, 1 });

		Render_Screen_Sprite(Coords.X1o, Coords.Y2o,
			Coords.X2o, Coords.Y2,
			{ 0, 0 }, { Delta_X_UV, 0 }, { 0, 1 }, { Delta_X_UV, 1 });

		//

		Render_Screen_Sprite(Coords.X1, Coords.Y1o, Coords.X1o, Coords.Y2o,
			{ 0, 0 }, { 0, 1 }, { Delta_Y_UV, 0 }, { Delta_Y_UV, 1 });

		Render_Screen_Sprite(Coords.X2o, Coords.Y1o, Coords.X2, Coords.Y2o,
			{ 0, 0 }, { 0, 1 }, { Delta_Y_UV, 0 }, { Delta_Y_UV, 1 });
	}

	virtual void Render(UI_Transformed_Coordinates Coords)
	{
		UI_Shader.Activate();
		
		if (Flags[UF_RENDER_BORDER])
			Render_Border(Coords);

		if (Flags[UF_IMAGE])
			Bind_UI_Uniforms(UI_Shader, Image, Colour);


		if(Flags[UF_RENDER_CONTENTS])
			Render_Screen_Sprite(Coords.X1o, Coords.Y1o, Coords.X2o, Coords.Y2o,
				{ 0, 1 }, { 1, 1 }, { 0, 0 }, { 1, 0 });
	}

	virtual void Update_UI()
	{
		UI_Transformed_Coordinates Coords(X1, Y1, X2, Y2, UI_Border_Size, Flags[UF_CLAMP_TO_SIDE], Flags[UF_FILL_SCREEN], Flags[UF_CLAMP_RIGHT]);

		Render(Coords);

		// To handle this UI, there's nothing else we have to do
	}
};


class Text_UI_Element : public UI_Element
{
public:
#ifndef USING_FREETYPE_FONT
	std::vector<uint32_t> Character_Indices; // These are the indices of the letters in the font
#else
	std::string Text;
#endif

	float Size;

	float Italic_Slant;

	bool Centered_X = false, Centered_Y = false;

	Font_Table::Font* Font;

	glm::vec3 Text_Colour;

	Text_UI_Element() {}

	Text_UI_Element(float X1p, float Y1p, float X2p, float Y2p, std::string Textp, glm::vec3 Text_Colourp = glm::vec3(1.0f, 1.0f, 1.0f), Font_Table::Font* Fontp = &Font_Georgia, float Sizep = 1.0f / 15.0f, float Italic_Slantp = 0.0f)
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;

#ifndef USING_FREETYPE_FONT
		Font_Table::Generate_Text_Indices(Textp.c_str(), &Character_Indices);
#else
		Text = Textp;
#endif

		Size = Sizep;

		Italic_Slant = Italic_Slantp;

		Text_Colour = Text_Colourp;

		Font = Fontp;
	}

	virtual void Render_Text(UI_Transformed_Coordinates Coords)
	{
		// 62 is the character index of the space

#ifndef USING_FREETYPE_FONT

		Billboard_Vertex_Buffer Letter(
			Coords.X1o + Size * Window_Aspect_Ratio,
			Coords.Y2o - Size,
			Coords.X1o + Size * 2 * Window_Aspect_Ratio,
			Coords.Y2o - Size * (1 + Font_Table::Character_Aspect_Ratio),

			glm::vec2(0, 0.2f), glm::vec2((155.0f/static_cast<float>(Font_Table::Character_Width))/static_cast<float>(Font_Table::Font_Grid_Width), 0.2f),
			glm::vec2(0, 0.0f), glm::vec2((155.0f/static_cast<float>(Font_Table::Character_Width))/static_cast<float>(Font_Table::Font_Grid_Width), 0.0f),

			Italic_Slant * Window_Aspect_Ratio
		);

		size_t Line_Length = static_cast<size_t>((Coords.X2o - Coords.X1o - Size * 2 * Window_Aspect_Ratio) / ((Size + 0.01f) * Window_Aspect_Ratio));

		glUniform1ui(glGetUniformLocation(Text_Shader.Program_ID, "Line_Length"), Line_Length);

		glUniform1f(glGetUniformLocation(Text_Shader.Program_ID, "Size_Of_Letter"), (Size + 0.01f) * Window_Aspect_Ratio);
		glUniform1f(glGetUniformLocation(Text_Shader.Program_ID, "Height_Of_Letter"), (Size) * (0.9f + Font_Table::Character_Aspect_Ratio));

		glUniform1uiv(glGetUniformLocation(Text_Shader.Program_ID, "Character_Indices"), Character_Indices.size(), Character_Indices.data());
		
		glDrawElementsInstanced(GL_TRIANGLES, Letter.Indices_Count, GL_UNSIGNED_INT, 0u, Character_Indices.size());

		Letter.Delete_Buffer();

#else
		float X_Offset = 0;
		float Y_Offset = 0;

		for (size_t W = 0; W < Text.length(); W++)
		{
			if (Text[W] != ' ')
			{
				Font_Table::Character Character = Font->Characters[Text[W]];

				Bind_UI_Uniforms(Text_Shader, Character.Glyph, Colour);

				float Left_X_Pos = Coords.X1o + (Character.Offset.x + X_Offset) * Window_Aspect_Ratio * Size * Font->Character_Pixel_To_Screen_Space
					+ Size * Window_Aspect_Ratio * 0.5f;
				float Top_Y_Pos = Coords.Y2o + (Character.Offset.y) * Size * Font->Character_Pixel_To_Screen_Space - Size * (1.5f + Y_Offset);

				float Right_X_Pos = Left_X_Pos + Character.Size.x * Window_Aspect_Ratio * Size * Font->Character_Pixel_To_Screen_Space;

				Billboard_Vertex_Buffer Letter(
					Left_X_Pos,
					Top_Y_Pos,
					Right_X_Pos,
					Top_Y_Pos - Character.Size.y * Size * Font->Character_Pixel_To_Screen_Space,
					glm::vec2(0, 0), glm::vec2(1, 0),
					glm::vec2(0, 1), glm::vec2(1, 1),
					Italic_Slant * Window_Aspect_Ratio * Character.Size.y * Font->Character_Pixel_To_Screen_Space
				);

				X_Offset += Character.Step;

				{
					Right_X_Pos += Character.Step * Window_Aspect_Ratio * Size * Font->Character_Pixel_To_Screen_Space;

					if (Right_X_Pos > Coords.X2o - Size * Window_Aspect_Ratio * 0.5f)
					{
						X_Offset = 0.0f;
						Y_Offset += 1.0f;
					}
				}

				glDrawElements(GL_TRIANGLES, Letter.Indices_Count, GL_UNSIGNED_INT, 0u);

				Letter.Delete_Buffer();
			}
			else
				X_Offset += Font->Characters['a'].Step;
		}

#endif
	}

	virtual void Render(UI_Transformed_Coordinates Coords) override
	{
		UI_Shader.Activate();

		if (Flags[UF_RENDER_BORDER])
			Render_Border(Coords);

		Render_Screen_Sprite(Coords.X1o, Coords.Y1o, Coords.X2o, Coords.Y2o,
			{ 0, 1 }, { 1, 1 }, { 0, 0 }, { 1, 0 });

		Text_Shader.Activate();

#ifndef USING_FREETYPE_FONT
		Bind_UI_Uniforms(Text_Shader, Font_Table::Font, Colour);
#endif

		Render_Text(Coords);
	}
};

class Textbox_UI_Element; // This just declares that the Textbox_UI_Element exists. The definition thereof is in UI_Textbox_Definitions.h

class Button_UI_Element : public UI_Element
{
public:
	void (*Button_Function)(UI_Element*);

	Button_UI_Element(float X1p, float Y1p, float X2p, float Y2p, void (*Button_Functionp)(UI_Element*), Texture Imagep = Texture())
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;

		Button_Function = Button_Functionp;

		Image = Imagep;

		Flags[UF_IMAGE] = Image.Texture_Created();
	}

	virtual void Update_UI() override
	{
		UI_Transformed_Coordinates Coords(X1, Y1, X2, Y2, UI_Border_Size, Flags[UF_CLAMP_TO_SIDE], Flags[UF_FILL_SCREEN], Flags[UF_CLAMP_RIGHT]);

		bool Hovering = Button_Hover(Coords);

		Colour = glm::vec4(1, 1, 1, 1.0f);

		if (Hovering)
			Colour.b = 0.75f;
		else
			Colour.b = 1.0f;

		if (Mouse_Inputs[0])
			Colour *= glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);

		Render(Coords);

		if (Hovering && Mouse_Unclick(0)) // If we're hovering over the button and the mouse just unclicked, 
			Button_Function(this);		// run the button-function
	}
};

class Button_Text_UI_Element : public Text_UI_Element
{
public:
	void (*Button_Function)(UI_Element*);

	Button_Text_UI_Element(float X1p, float Y1p, float X2p, float Y2p, void (*Button_Functionp)(UI_Element*), std::string Textp, glm::vec3 Text_Colourp = glm::vec3(1.0f), Font_Table::Font* Fontp = &Font_Georgia, float Sizep = 1.0f / 15.0f, float Italic_Slantp = 0.0f)
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;

#ifndef USING_FREETYPE_FONT
		Font_Table::Generate_Text_Indices(Textp.c_str(), &Character_Indices);
#else
		Text = Textp;
#endif

		Size = Sizep;

		Text_Colour = Text_Colourp;

		Italic_Slant = Italic_Slantp;

		Button_Function = Button_Functionp;

		Font = Fontp;
	}

	virtual void Update_UI() override
	{
		UI_Transformed_Coordinates Coords(X1, Y1, X2, Y2, UI_Border_Size, Flags[UF_CLAMP_TO_SIDE], Flags[UF_FILL_SCREEN], Flags[UF_CLAMP_RIGHT]);

		bool Hovering = Button_Hover(Coords);

		Colour = glm::vec4(1, 1, 1, 1.0f);

		if (Hovering)
			Colour.b = 0.75f;
		else
			Colour.b = 1.0f;

		if (Hovering && Mouse_Inputs[0])
			Colour *= glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);

		Render(Coords);

		if (Hovering && Mouse_Unclick(0)) // If we're hovering over the button and the mouse just unclicked, 
			Button_Function(this);		// run the button-function
	}
};

void Return_To_Game_Loop(UI_Element* Element)
{
	UI_Continue_Looping = false;
	Cursor_Reset = true;
}

std::vector<UI_Element*> UI_Elements;

// Will need some hash tables and functions for handling text rendering

// Get a conversion thingy from character to index into font

void Handle_UI() // If there are transparent UI elements (pretty likely), then we're gonna want to render this in a final transparent rendering pass
{
	glDisable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (size_t W = 0; W < UI_Elements.size(); W++)
		UI_Elements[W]->Update_UI();
}

#endif
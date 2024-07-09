#ifndef VISION_ENGINE_UI_RENDERER_DECLARATIONS
#define VISION_ENGINE_UI_RENDERER_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Texture_Declarations.h"
#include "Asset_Loading_Cache.h"

#define UF_TO_BE_DELETED 0u
#define UF_RENDER_BORDER 1u

Shader UI_Shader;

void Initialise_UI_Shaders()
{
	UI_Shader.Create_Shader("Shader_Code/UI_Shader.vert", "Shader_Code/UI_Shader.frag", nullptr);
}

void Bind_UI_Uniforms(Texture Image, glm::vec4 Colour)
{
	glUniform4f(glGetUniformLocation(UI_Shader.Program_ID, "Colour"), Colour.x, Colour.y, Colour.z, Colour.w);

	Image.Parse_Texture(UI_Shader, "Albedo", 0u);
	Image.Bind_Texture();
}

void Render_Screen_Sprite(float X1, float Y1, float X2, float Y2, glm::vec2 TL_UV, glm::vec2 TR_UV, glm::vec2 BL_UV, glm::vec2 BR_UV)
{
	// Make vertex buffer for this

	Billboard_Vertex_Buffer Vertex_Buffer(X1, Y1, X2, Y2, TL_UV, TR_UV, BL_UV, BR_UV); // This draws the image within the UI border

	Vertex_Buffer.Bind_Buffer();

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
public:
	float X1, Y1, X2, Y2; // Bounds

	float X1o, Y1o, X2o, Y2o; // Offset
	UI_Transformed_Coordinates(float X1p, float Y1p, float X2p, float Y2p, float UI_Border_Size)
	{
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

	bool Flags[2] = { false, true };

	const float UI_Border_Size = 1.0f / 20.0f;

	const float Inv_UI_Border_Size = 20.0f;

	// X1 and Y1 are the top-left corner of the screen

	// X2 and Y2 are the bottom-right corner of the screen

	UI_Element(float X1p, float Y1p, float X2p, float Y2p)
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;
	}

	virtual void Render_Border(UI_Transformed_Coordinates Coords)
	{
		Bind_UI_Uniforms(Pull_Texture("Assets/UI/UI_Corner.png").Texture, glm::vec4(1, 1, 1, 0.5f));

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

		Bind_UI_Uniforms(Pull_Texture("Assets/UI/UI_Line.png").Texture, glm::vec4(1, 1, 1, 0.5f));

		float Delta_X_UV = (Coords.X2 - Coords.X1) * Inv_UI_Border_Size - 2;
		float Delta_Y_UV = (Coords.Y2 - Coords.Y1) * Inv_UI_Border_Size - 2;

		Render_Screen_Sprite(Coords.X1o, Coords.Y1,
			Coords.X2o, Coords.Y2o, 
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

	virtual void Render()
	{
		UI_Shader.Activate();

		UI_Transformed_Coordinates Coords(X1, Y1, X2, Y2, UI_Border_Size);
		
		if (Flags[UF_RENDER_BORDER])
			Render_Border(Coords);

		Render_Screen_Sprite(Coords.X1o, Coords.Y1o, Coords.X2o, Coords.Y2o,
			{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 });
	}

	virtual void Update_UI()
	{
		Render();

		// To handle this UI, there's nothing else we have to do
	}
};

std::vector<UI_Element*> UI_Elements;

void Handle_UI() // If there are transparent UI elements (pretty likely), then we're gonna want to render this in a final transparent rendering pass
{
	for (size_t W = 0; W < UI_Elements.size(); W++)
		UI_Elements[W]->Update_UI();
}

#endif
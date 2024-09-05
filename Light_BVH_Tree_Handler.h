#ifndef VISION_LIGHT_BVH_TREE_HANDLER
#define VISION_LIGHT_BVH_TREE_HANDLER

#include "OpenGL_Declarations.h"
#include "Lighting_Handler.h"

// We will use integers to store the positions of the boundaries to save memory
// Each leaf node will have ~8 light sources, perhaps more
// The scene will either have 64-128 light sources at once.

// Later, I might make changes to the shadow mapping to allow for more than 1 shadow-mapped light source at once.

/*

		0			depth = 0
	  /	  \
	 1	   2		depth = 1
	/ \   / \
   3   4  5  6		depth = 2

   This tree has a depth of 3

   usw.

   number of nodes in binary tree = 2^(depth) - 1	or		(1u << depth) - 1u

   The index of the next node is always 2n + 1 + (side_of_partition).

   z.B. The left of 2 is equal to 2(2) + 1 + 0 = 5,
   and	the right of 2 is equal to 2(2) + 1 + 1 = 6

   and you can see this in the graph as well
*/

namespace Lighting_BVH // This uses considerably less memory than my previous design of the lighting BVH tree
{
#define Boundary_Max_Value 15.0f

	struct Node_Partition
	{
		float X = Boundary_Max_Value, Y = Boundary_Max_Value;
	};
	struct Leaf_Node
	{
		unsigned char Light_Indices[8]; // Each leaf node should have 8 light indices
	};

	struct Leaf_Node_Bounds // This is used only by the CPU-side to assess each node's optimal lights
	{
		glm::vec2 Position = glm::vec2(99999, 99999); // This is the assessed position of each node - changes can be made to this manually according to artist parameters
	};

	const size_t Binary_Tree_Depth = 7; // How many layers in the tree there are

	constexpr const size_t Number_Of_Partition_Nodes = (1u << (Binary_Tree_Depth - 1u)) - 1u;

	Node_Partition Partition_Nodes[Number_Of_Partition_Nodes]; // These are the structures that are passed to the GPU

	constexpr const size_t Number_Of_Leaf_Nodes = (1u << (Binary_Tree_Depth - 1u));

	Leaf_Node Leaf_Nodes[Number_Of_Leaf_Nodes]; // This structure is passed to the GPU, giving the indices of each leaf nodes' lights.

	//

	Leaf_Node_Bounds Leaf_Nodes_Info[Number_Of_Leaf_Nodes]; // Again, this is entirely CPU-side, used to assess each node's optimal lights

	//

	struct Light_Occluder
	{
		glm::vec2 A, B;
	};

	std::vector<Light_Occluder> Light_Occluders; // These are the light occluders, added by the level designer

	bool Occluded(glm::vec2 Node_Position, glm::vec2 Light_Position)
	{
		// This just finds if there's a collision between the two

		glm::vec2 Delta = Light_Position - Node_Position;

		for (size_t W = 0; W < Light_Occluders.size(); W++)
		{

		}

		return false;
	}

	void Update_Leaf_Node_Data();

	void Parse_Partition_Nodes_To_Shader(Shader& Shader)
	{
		glUniform2fv(glGetUniformLocation(Shader.Program_ID, "Partition_Nodes"), Number_Of_Partition_Nodes, (const GLfloat*)Partition_Nodes);

		glUniform1uiv(glGetUniformLocation(Shader.Program_ID, "Leaf_Node_Indices"), Number_Of_Leaf_Nodes * 2, (const GLuint*)Leaf_Nodes);

		// We specifically use a count of (Number_Of_Partition_Nodes / 4) because we pack the integers in such a way that each byte is stored in 1/4th of a full 32-bit word
	}

	void Generate_Light_BVH_Tree();

	void Update_Leaf_Node_Data_Threaded(void* Parameters)
	{
		size_t* Offset = (size_t*)Parameters;


		for (size_t W = *Offset; W < Number_Of_Leaf_Nodes; W += NUMBER_OF_WORKERS)
		{
			struct Lightsource_Index_Data
			{
				unsigned char Index;
				float Distance;

				bool operator<(Lightsource_Index_Data& Other)
				{
					return Distance < Other.Distance;
				}
			};

			std::vector<Lightsource_Index_Data> Index_Data(Scene_Lights.size());

			for (size_t V = 0; V < Scene_Lights.size(); V++)
			{
				Index_Data[V].Index = V;
				Index_Data[V].Distance = squaref(Scene_Lights[V]->Position.x - Leaf_Nodes_Info[W].Position.x) + squaref(Scene_Lights[V]->Position.z - Leaf_Nodes_Info[W].Position.y);

				Index_Data[V].Distance += 1000 * Occluded(Leaf_Nodes_Info[W].Position, 
					glm::vec2(Scene_Lights[V]->Position.x, Scene_Lights[V]->Position.z));

				// If the light is occluded, it can be treated as though it is much lower priority than it is
			}

			std::sort(Index_Data.begin(), Index_Data.end());

			for (size_t Indices = 0; Indices < 8; Indices++)
				Leaf_Nodes[W].Light_Indices[Indices] = Index_Data[Indices].Index;
		}

		delete Offset;
	}

	void Update_Leaf_Node_Data()
	{
		for (size_t W = 0; W < NUMBER_OF_WORKERS - 1u; W++)
			Job_System::Submit_Job(Job_System::Job(Update_Leaf_Node_Data_Threaded, new size_t(W)));

		Update_Leaf_Node_Data_Threaded(new size_t(NUMBER_OF_WORKERS - 1u));
	}

	bool Determine_Side_Of_Node(float X, float Y, Node_Partition Node)
	{
		//return (Node.X < BVH_Float_To_Byte(X)) || 
		//	(Node.Y < BVH_Float_To_Byte(Y));

		return (Node.X < X || Node.Y < Y);

		// If the x coordinate is further right than the boundary OR the y coordinate is further up than the boundary, it's the right-hand child node in the binary tree

		// For this reason, default (or unimportant) values in the BVH tree will be set to 127 (the highest positive signed char value)
	}

	bool Validate_Position_Upwards_Traverse(float X, float Y, size_t Index)
	{
		if (Index)
		{
			size_t Parent_Index = (Index - 1) >> 1;

			bool Correct_Side = ((Index + 1) & 1u) == Determine_Side_Of_Node(X, Y, Partition_Nodes[Parent_Index]);

			if (Correct_Side)
				return Validate_Position_Upwards_Traverse(X, Y, Parent_Index);
			else
				return false;
		}
		else
			return true;
	}

	struct Boundary
	{
		float Min_X = -Boundary_Max_Value, 
			Max_X = Boundary_Max_Value, 
			Min_Y = -Boundary_Max_Value, 
			Max_Y = Boundary_Max_Value;
		//char Min_X = -128, Max_X = 127, Min_Y = -128, Max_Y = 127;
	};

	std::vector<Lightsource*> Get_All_Lightsources_In_Node(Boundary Volume)
	{
		std::vector<Lightsource*> List_Of_Lights;

		for (size_t W = 0; W < Scene_Lights.size(); W++)
			if (
				Scene_Lights[W]->Position.x > Volume.Min_X &&
				Scene_Lights[W]->Position.x < Volume.Max_X &&
				Scene_Lights[W]->Position.z > Volume.Min_Y &&
				Scene_Lights[W]->Position.z < Volume.Max_Y)
				List_Of_Lights.push_back(Scene_Lights[W]);

		return List_Of_Lights;
	}

	void Generate_Light_BVH_Tree() // The light BVH tree will probably need to be updated on a semi-regular basis, every half second at least
	{
		Boundary Boundaries[Number_Of_Partition_Nodes + Number_Of_Leaf_Nodes];

		for (size_t W = 0; W < Number_Of_Partition_Nodes; W++)
		{
			// This will decide, for each node, where the partition should go based on all of the found lights within this node

			std::vector<Lightsource*> Lightsources = Get_All_Lightsources_In_Node(Boundaries[W]);

			size_t Child_Node = (W << 1) + 1;

			glm::vec2 Min = glm::vec2(Boundaries[W].Min_X, Boundaries[W].Min_Y),
				Max = glm::vec2(Boundaries[W].Max_X, Boundaries[W].Max_Y),
				Middle;

			if (Lightsources.size() > 1)
			{
				Min = glm::vec2(Lightsources[0]->Position.x, Lightsources[0]->Position.z);

				Max = Min;

				for (size_t V = 1; V < Lightsources.size(); V++)
				{
					Min.x = std::fminf(Min.x, Lightsources[V]->Position.x);
					Min.y = std::fminf(Min.y, Lightsources[V]->Position.z);

					Max.x = std::fmaxf(Max.x, Lightsources[V]->Position.x);
					Max.y = std::fmaxf(Max.y, Lightsources[V]->Position.z);
				}
			}

			Middle = Min + Max;
			Middle *= 0.5f;

			if ((Max.x - Min.x) >= (Max.y - Min.y))
			{
				Partition_Nodes[W].X = Middle.x;// BVH_Float_To_Byte(Middle.x);
				Partition_Nodes[W].Y = Boundary_Max_Value; // 127;

				Boundaries[Child_Node] = Boundaries[W];

				Boundaries[Child_Node].Max_X = Partition_Nodes[W].X;

				//

				Boundaries[Child_Node + 1] = Boundaries[W];

				Boundaries[Child_Node + 1].Min_X = Partition_Nodes[W].X;
			}
			else
			{
				Partition_Nodes[W].X = Boundary_Max_Value;
				Partition_Nodes[W].Y = Middle.y;// BVH_Float_To_Byte(Middle.y);

				Boundaries[Child_Node] = Boundaries[W];
				Boundaries[Child_Node].Max_Y = Partition_Nodes[W].Y;

				Boundaries[Child_Node + 1] = Boundaries[W];
				Boundaries[Child_Node + 1].Min_Y = Partition_Nodes[W].Y;
			}
		}

		for (size_t W = Number_Of_Partition_Nodes; W < Number_Of_Partition_Nodes + Number_Of_Leaf_Nodes; W++)
		{
			float Conversion = 1.0f / 5.0f;

			if(false)
			{
				UI_Elements.push_back(new UI_Element(Conversion * Boundaries[W].Min_X, Conversion * Boundaries[W].Min_Y, Conversion * Boundaries[W].Max_X, Conversion * Boundaries[W].Max_Y, Pull_Texture("Assets/Textures/Pillow.png").Texture));
				UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
				UI_Elements.back()->UI_Border_Size = 0.0f;
			}

			//

			Leaf_Nodes_Info[W - Number_Of_Partition_Nodes].Position.x = (Boundaries[W].Max_X + Boundaries[W].Min_X) * 0.5f;
			Leaf_Nodes_Info[W - Number_Of_Partition_Nodes].Position.y = (Boundaries[W].Max_Y + Boundaries[W].Min_Y) * 0.5f;
		}

		// From there, we'll generate all the leaf node's average positions
	}
}

#endif
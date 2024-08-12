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
	float BVH_Bounds = 32.0f;
	float BVH_Conversion = 4.0f;
	float BVH_Inverse_Conversion = 0.25f;

	void Set_Bounds(float Bounds) // You can consider bounds as the "radius" of the map's BVH 
	{
		BVH_Bounds = Bounds;

		BVH_Conversion = 128.0f / Bounds;
		BVH_Inverse_Conversion = Bounds / 128.0f;
	}

	struct Node_Partition
	{
		char X, Y; // We will be using 8-bit integers for this to reduce memory sent to the GPU

		// Goes between 127 to -128
	};

	float BVH_Byte_To_Float(char X) 
	{
		return (float)(X) * BVH_Inverse_Conversion;
	}

	char BVH_Float_To_Byte(float X)
	{
		return (char)(X * BVH_Conversion);
	}

	struct Leaf_Node
	{
		unsigned char Light_Indices[8]; // Each leaf node should have 8 light indices
	};

	struct Leaf_Node_Bounds // This is used only by the CPU-side to assess each node's optimal lights
	{
		glm::vec2 Position; // This is the assessed position of each node - changes can be made to this manually according to artist parameters
	};

	const size_t Binary_Tree_Depth = 5; // How many layers in the tree there are

	constexpr const size_t Number_Of_Partition_Nodes = (1u << (Binary_Tree_Depth - 1u)) - 1u;

	Node_Partition Partition_Nodes[Number_Of_Partition_Nodes]; // These are the structures that are passed to the GPU

	constexpr const size_t Number_Of_Leaf_Nodes = (1u << (Binary_Tree_Depth - 1u));

	Leaf_Node Leaf_Nodes[Number_Of_Leaf_Nodes]; // This structure is passed to the GPU, giving the indices of each leaf nodes' lights.

	//

	Leaf_Node_Bounds Leaf_Nodes_Info[Number_Of_Leaf_Nodes]; // Again, this is entirely CPU-side, used to assess each node's optimal lights

	void Parse_Partition_Nodes_To_Shader(Shader& Shader)
	{
		glUniform1uiv(glGetUniformLocation(Shader.Program_ID, "Partition_Nodes"), Number_Of_Partition_Nodes >> 2, (const GLuint*)Partition_Nodes);

		// We specifically use a count of (Number_Of_Partition_Nodes / 4) because we pack the integers in such a way that each byte is stored in 1/4th of a full 32-bit word
	}

	void Update_Leaf_Node_Data()
	{
		for (size_t W = 0; W < Number_Of_Leaf_Nodes; W++)
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
			}

			std::sort(Index_Data.begin(), Index_Data.end());

			for (size_t Indices = 0; Indices < 8; Indices++)
				Leaf_Nodes[W].Light_Indices[Indices] = Index_Data[Indices].Index;
		}
	}

	void Wipe_Light_BVH_Tree() // This will clear all of the associated light BVH tree data
	{
		
	}

	bool Determine_Side_Of_Node(float X, float Y, Node_Partition Node)
	{
		return (Node.X < BVH_Float_To_Byte(X)) || 
			(Node.Y < BVH_Float_To_Byte(Y));

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

	std::vector<Lightsource*> Get_All_Lightsources_In_Node(size_t Node_Index)
	{
		// We'll traverse the tree upwards

		std::vector<Lightsource*> List_Of_Lights;

		for (size_t W = 0; W < Scene_Lights.size(); W++)
			if (Validate_Position_Upwards_Traverse(Scene_Lights[W]->Position.x, Scene_Lights[W]->Position.z, Node_Index))
				List_Of_Lights.push_back(Scene_Lights[W]);

		return List_Of_Lights;
	}

	struct Boundary
	{
		char Min_X = -128, Max_X = 127, Min_Y = -128, Max_Y = 127;
	};

	void Generate_Light_BVH_Tree() // The light BVH tree will probably need to be updated on a semi-regular basis, every half second at least
	{
		Boundary Boundaries[Number_Of_Partition_Nodes + Number_Of_Leaf_Nodes];

		for (size_t W = 0; W < Number_Of_Partition_Nodes; W++)
		{
			// This will decide, for each node, where the partition should go based on all of the found lights within this node

			std::vector<Lightsource*> Lightsources = Get_All_Lightsources_In_Node(W);

			size_t Child_Node = (W << 1) + 1;

			if (Lightsources.size())
			{
				glm::vec2 Min = glm::vec2(Lightsources[0]->Position.x, Lightsources[0]->Position.z);

				glm::vec2 Max = Min;

				for (size_t V = 1; V < Lightsources.size(); V++)
				{
					Min.x = std::fminf(Min.x, Lightsources[V]->Position.x);
					Min.y = std::fminf(Min.y, Lightsources[V]->Position.z);

					Max.x = std::fmaxf(Max.x, Lightsources[V]->Position.x);
					Max.y = std::fmaxf(Max.y, Lightsources[V]->Position.z);
				}

				glm::vec2 Middle = Min + Max;
				Middle *= 0.5f;

				if (Max.x - Min.x > Max.y - Min.y)
				{
					Partition_Nodes[W].X = BVH_Float_To_Byte(Middle.x);
					Partition_Nodes[W].Y = 127;

					Boundaries[Child_Node] = Boundaries[W];

					Boundaries[Child_Node].Max_X = Partition_Nodes[W].X;

					//

					Boundaries[Child_Node + 1] = Boundaries[W];

					Boundaries[Child_Node + 1].Min_X = Partition_Nodes[W].X;
				}
				else
				{
					Partition_Nodes[W].X = 127;
					Partition_Nodes[W].Y = BVH_Float_To_Byte(Middle.y);

					Boundaries[Child_Node] = Boundaries[W];
					Boundaries[Child_Node].Max_Y = Partition_Nodes[W].Y;

					Boundaries[Child_Node + 1] = Boundaries[W];
					Boundaries[Child_Node + 1].Min_Y = Partition_Nodes[W].Y;
				}
			}
			else
			{
				Partition_Nodes[W].X = 127;
				Partition_Nodes[W].Y = Boundaries[W].Min_Y + ((Boundaries[W].Max_Y - Boundaries[W].Min_Y) >> 1);

				Boundaries[Child_Node] = Boundaries[W];
				Boundaries[Child_Node].Max_Y = Partition_Nodes[W].Y;

				Boundaries[Child_Node + 1] = Boundaries[W];
				Boundaries[Child_Node + 1].Min_Y = Partition_Nodes[W].Y;
			}
		}

		for (size_t W = Number_Of_Partition_Nodes; W < Number_Of_Partition_Nodes + Number_Of_Leaf_Nodes; W++)
		{
			float Conversion = 1.0f / 200.0f;

			// UI_Elements.push_back(new UI_Element(Conversion * Boundaries[W].Min_X, Conversion * Boundaries[W].Min_Y, Conversion * Boundaries[W].Max_X, Conversion * Boundaries[W].Max_Y, Pull_Texture("Assets/Textures/Gun_Texture.png").Texture));
			// UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
			// UI_Elements.back()->UI_Border_Size = 0.0f;

			//

			Leaf_Nodes_Info[W - Number_Of_Partition_Nodes].Position.x = (Boundaries[W].Max_X + Boundaries[W].Min_X) * 0.5f;
			Leaf_Nodes_Info[W - Number_Of_Partition_Nodes].Position.y = (Boundaries[W].Max_Y + Boundaries[W].Min_Y) * 0.5f;
		}

		// From there, we'll generate all the leaf node's average positions
	}
}

#endif
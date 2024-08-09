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
	struct Node_Partition
	{
		char X, Y; // We will be using 8-bit integers for this to reduce memory sent to the GPU
	};

	struct Leaf_Node
	{
		unsigned char Light_Indices[8]; // Each leaf node should have 8 light indices
	};

	struct Leaf_Node_Bounds // This is used only by the CPU-side to assess each node's optimal lights
	{
		glm::vec2 Position; // This is the assessed position of each node - changes can be made to this manually according to artist parameters
	};

	const size_t Binary_Tree_Depth = 6; // How many layers in the tree there are

	constexpr const size_t Number_Of_Partition_Nodes = (1u << (Binary_Tree_Depth - 1u)) - 1u;

	constexpr Node_Partition Partition_Nodes[Number_Of_Partition_Nodes]; // These are the structures that are passed to the GPU

	constexpr const size_t Number_Of_Leaf_Nodes = (1u << (Binary_Tree_Depth - 1u));

	constexpr Leaf_Node Leaf_Nodes[Number_Of_Leaf_Nodes]; // This structure is passed to the GPU, giving the indices of each leaf nodes' lights.

	//

	constexpr Leaf_Node_Bounds Leaf_Nodes_Info[Number_Of_Leaf_Nodes]; // Again, this is entirely CPU-side, used to assess each node's optimal lights

	void Wipe_Light_BVH_Tree() // This will clear all of the associated light BVH tree data
	{

	}

	void Generate_Light_BVH_Tree() // The light BVH tree will probably need to be updated on a semi-regular basis, every half second at least
	{

	}
}

#endif
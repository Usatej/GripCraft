#pragma once

namespace CraftMeshUtils
{
	static const FIntVector Directions[6] = {
		FIntVector(1, 0, 0),		//right
		FIntVector(-1, 0, 0),	//left
		FIntVector(0, 1, 0),		//front
		FIntVector(0, -1, 0),	//back
		FIntVector(0, 0, 1),		//top
		FIntVector(0, 0, -1)};	//bottom
	
	/*
	 *		 ^
	 *		 | Z
	 *		
	 *    0 ------ 1
	 *   /|      / |     X
	 *	3 ----- 2  |    --->
	 *  | 4 ----|- 5
	 *  |/      | /
	 *  7 ----- 6
	 *
	 *	   / 
	 *	  / Y
	 *	 V
	 *
	 */
	
	static const FVector VertexPositions[8] = {
		FVector(-1, -1, 1),
		FVector(1, -1, 1),
		FVector(1, 1, 1),
		FVector(-1, 1, 1),
		FVector(-1, -1, -1),
		FVector(1, -1, -1),
		FVector(1, 1, -1),
		FVector(-1, 1, -1),
	};
	
	 static const int Faces[6][4] = {
	 	{2, 6, 5, 1},	//right
	 	{0, 4, 7, 3},	//left
	 	{3, 7, 6, 2},	//front
	 	{1, 5, 4, 0},	//back
	 	{0, 3, 2, 1},	//top
	 	{7, 4, 5, 6}	//bottom
	};
	
}
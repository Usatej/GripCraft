#pragma once

class ACraftWorldChunk;

struct FCraftChunkNeighbours
{
	TObjectPtr<ACraftWorldChunk> Right;
	TObjectPtr<ACraftWorldChunk> Left;
	TObjectPtr<ACraftWorldChunk> Front;
	TObjectPtr<ACraftWorldChunk> Back;
};
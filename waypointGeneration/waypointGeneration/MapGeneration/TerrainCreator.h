#pragma once
#include "../Rendering/structs.h"

class TerrainCreator
{
public:
	TerrainCreator();
	~TerrainCreator();
	
	std::vector<Vertex> CreateTerrainFromFloatList(std::vector<float> heightValues, int mapSize);

private:

};
#pragma once
#include "../Rendering/structs.h"

class TerrainCreator
{
public:
	const float SNOW_START = 20.0f;
	const float WATER_START = -5.0f;
	const float UNWALKABLE_SURFACE = 0.7f;

	TerrainCreator();
	~TerrainCreator();
	
	std::vector<Vertex> CreateTerrainFromFloatList(std::vector<float> heightValues, int mapSize, ID3D11ShaderResourceView *& srv, ID3D11Texture2D *& tex2D);
	std::vector<Vertex> CreateTerrainFromFloatList2(std::vector<float> heightValues, int mapSize, ID3D11ShaderResourceView *& srv, ID3D11Texture2D *& tex2D, __out std::vector<Vertex> edges[4]);
};
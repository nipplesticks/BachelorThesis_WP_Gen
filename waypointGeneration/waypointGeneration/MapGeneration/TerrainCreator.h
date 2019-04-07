#pragma once
#include "../Rendering/structs.h"

class TerrainCreator
{
public:
	const float SNOW_START = 20.0f;
	const float WATER_START = -5.0f;
	const float UNWALKABLE_SURFACE = 0.8f;

	TerrainCreator();
	~TerrainCreator();
	
	std::vector<Vertex> CreateTerrainFromFloatList(std::vector<float> heightValues, int mapSize, ID3D11ShaderResourceView *& srv, ID3D11Texture2D *& tex2D);
	std::vector<Vertex> CreateTerrainFromFloatList2(std::vector<float> heightValues, int mapSize);

private:
	void _normalSmoothingType0(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType1(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType2(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType3(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType4(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType5(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);

};
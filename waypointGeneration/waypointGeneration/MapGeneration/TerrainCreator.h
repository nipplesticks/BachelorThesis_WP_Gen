#pragma once
#include "../Rendering/structs.h"

class TerrainCreator
{
public:
	TerrainCreator();
	~TerrainCreator();
	
	std::vector<Vertex> CreateTerrainFromFloatList(std::vector<float> heightValues, int mapSize);
	std::vector<Vertex> CreateTerrainFromFloatList2(std::vector<float> heightValues, int mapSize);

private:
	void _normalSmoothingType0(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType1(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType2(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType3(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType4(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);
	void _normalSmoothingType5(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex> & vertices, int index, int mapSize);

};
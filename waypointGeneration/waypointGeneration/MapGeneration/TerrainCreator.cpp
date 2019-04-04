#include "waypointGenerationPCH.h"
#include "TerrainCreator.h"

TerrainCreator::TerrainCreator()
{
}

TerrainCreator::~TerrainCreator()
{
}

std::vector<Vertex> TerrainCreator::CreateTerrainFromFloatList(std::vector<float> heightValues, int mapSize)
{
	std::vector<Vertex> vertices;

	for (int i = 0; i < mapSize - 1; i++)
	{
		for (int j = 0; j < mapSize - 1; j++)
		{
			Vertex v;

			v.Position = DirectX::XMFLOAT4A(float(j), float(heightValues[j + i * mapSize]), float(i), 1.0f);
			v.Normal = DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, 0.0f);
			vertices.push_back(v);

			v.Position = DirectX::XMFLOAT4A(float(j), float(heightValues[j + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			v.Position = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + i * mapSize]), float(i), 1.0f);
			vertices.push_back(v);

			v.Position = DirectX::XMFLOAT4A(float(j), float(heightValues[j + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			v.Position = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			v.Position = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + i * mapSize]), float(i), 1.0f);
			vertices.push_back(v);
		}
	}

	return vertices;
}

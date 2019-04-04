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

	std::vector<DirectX::XMFLOAT4A> triangleNormals;
	std::vector<Vertex> vertices;


	int counter = 0;
	for (int i = 0; i < mapSize - 1; i++)
	{
		for (int j = 0; j < mapSize - 1; j++)
		{
			Vertex v;

			DirectX::XMFLOAT4A v0, v1, v2;

			v.Position = v0 = DirectX::XMFLOAT4A(float(j), float(heightValues[j + i * mapSize]), float(i), 1.0f);
			v.Normal = DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, 0.0f);
			vertices.push_back(v);

			v.Position = v1 = DirectX::XMFLOAT4A(float(j), float(heightValues[j + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			v.Position = v2 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + i * mapSize]), float(i), 1.0f);
			vertices.push_back(v);

			DirectX::XMVECTOR e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1), DirectX::XMLoadFloat4A(&v0));
			DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2), DirectX::XMLoadFloat4A(&v0));
			DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));

			for (int k = vertices.size() - 4; k < vertices.size(); k++)
				DirectX::XMStoreFloat4A(&vertices[k].Normal, normal);

			triangleNormals.push_back(vertices.back().Normal);

			v.Position = v0 = DirectX::XMFLOAT4A(float(j), float(heightValues[j + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			v.Position = v1 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			v.Position = v2 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + i * mapSize]), float(i), 1.0f);
			vertices.push_back(v);

			e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1), DirectX::XMLoadFloat4A(&v0));
			e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2), DirectX::XMLoadFloat4A(&v0));
			normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));

			for (int k = vertices.size() - 4; k < vertices.size(); k++)
				DirectX::XMStoreFloat4A(&vertices[k].Normal, normal);

			triangleNormals.push_back(vertices.back().Normal);
		}
	}

	for (auto & v : vertices)
	{
		bool done = false;
		auto it = vertices.begin();
		DirectX::XMFLOAT4A normal(0, 0, 0, 0);
		int normalCounter = 0;

		it = std::find(it, vertices.end(), v);
		while (!done)
		{

			if (it != vertices.end())
			{
				normalCounter++;
				size_t index = it - vertices.begin();
				const DirectX::XMFLOAT4A & nor = triangleNormals[index / 3];

				normal.x += nor.x;
				normal.y += nor.y;
				normal.z += nor.z;
				it = std::find(it + 1, vertices.end(), v);
			}
			else
			{
				done = true;
			}
		}
		normal.x /= (float)normalCounter;
		normal.y /= (float)normalCounter;
		normal.z /= (float)normalCounter;
		v.Normal = normal;
	}

	return vertices;
}

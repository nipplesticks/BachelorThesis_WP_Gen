#include "waypointGenerationPCH.h"
#include "TerrainCreator.h"
#include <map>

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

	return vertices;
}

std::vector<Vertex> TerrainCreator::CreateTerrainFromFloatList2(std::vector<float> heightValues, int mapSize)
{
	std::vector<DirectX::XMFLOAT4A> triangleNormals;
	std::vector<Vertex> vertices;

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

	for (int i = 0; i < vertices.size(); i++)
	{
		int type = i % 6;

		switch (type)
		{
		case 0:
			_normalSmoothingType0(triangleNormals, vertices, i, mapSize);
			break;
		case 1:
			_normalSmoothingType1(triangleNormals, vertices, i, mapSize);
			break;
		case 2:
			_normalSmoothingType2(triangleNormals, vertices, i, mapSize);
			break;
		case 3:
			_normalSmoothingType3(triangleNormals, vertices, i, mapSize);
			break;
		case 4:
			_normalSmoothingType4(triangleNormals, vertices, i, mapSize);
			break;
		case 5:
			_normalSmoothingType5(triangleNormals, vertices, i, mapSize);
			break;
		}
	}

	return vertices;
}

DirectX::XMFLOAT4A xmAdd(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
{
	DirectX::XMFLOAT4A r;
	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	r.w = 1.0f;

	return r;
}

DirectX::XMFLOAT4A xmAvg(const DirectX::XMFLOAT4A & a, int nrOf)
{
	DirectX::XMFLOAT4A r = a;
	r.x /= nrOf;
	r.y /= nrOf;
	r.z /= nrOf;

	return r;
}

void TerrainCreator::_normalSmoothingType0(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex>& vertices, int index, int mapSize)
{
	int counter = 1;
	DirectX::XMFLOAT4A normal = normals[index / 3];

	if (index > 0)
	{
		counter++;
		normal = xmAdd(normal, normals[(index - 1) / 3]);
	}

	if (index < vertices.size())
	{
		counter++;
		normal = xmAdd(normal, normals[(index + 1) / 3]);
	}

}

void TerrainCreator::_normalSmoothingType1(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex>& vertices, int index, int mapSize)
{
}

void TerrainCreator::_normalSmoothingType2(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex>& vertices, int index, int mapSize)
{
}

void TerrainCreator::_normalSmoothingType3(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex>& vertices, int index, int mapSize)
{
}

void TerrainCreator::_normalSmoothingType4(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex>& vertices, int index, int mapSize)
{
}

void TerrainCreator::_normalSmoothingType5(std::vector<DirectX::XMFLOAT4A> & normals, std::vector<Vertex>& vertices, int index, int mapSize)
{
}

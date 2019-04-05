#include "waypointGenerationPCH.h"
#include "TerrainCreator.h"
#include <map>
#include "../Rendering/Rendering/Renderer.h"


TerrainCreator::TerrainCreator()
{
	
}

TerrainCreator::~TerrainCreator()
{
}

bool float4Equal(const DirectX::XMFLOAT4 & a, const DirectX::XMFLOAT4 & b)
{
	if (fabs(a.x - b.x) > 0.01f)
		return false;
	if (fabs(a.y - b.y) > 0.01f)
		return false;
	if (fabs(a.y - b.y) > 0.01f)
		return false;

	return true;
}

std::vector<Vertex> TerrainCreator::CreateTerrainFromFloatList(std::vector<float> heightValues, int mapSize, ID3D11ShaderResourceView *& srv, ID3D11Texture2D *& tex2D)
{
	static const float SNOW_START = 10.0f;
	static const float WATER_START = -5.0f;

	srv = nullptr;
	tex2D = nullptr;
	std::vector<DirectX::XMFLOAT4A> triangleNormals;
	std::vector<Vertex> vertices;
	std::multimap<int, int> vertexMap;

	DirectX::XMFLOAT4 colors[] = {
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.37647f, 0.5019f, 0.219607f, 1.0f},
		//{0.0f, 1.0f, 0.0f, 1.0f},
		//{1.0f, 0.0f, 0.0f, 1.0f}
		{0.62745f, 0.321569f, 0.1764706f, 1.0f}
	};

	std::vector<DirectX::XMFLOAT4> texture(mapSize * mapSize);
	
	DirectX::XMVECTOR uvStart = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR uvEnd = DirectX::XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);

	int counter = 0;
	for (int i = 0; i < mapSize - 1; i++)
	{	
		for (int j = 0; j < mapSize - 1; j++)
		{
			Vertex v;

			DirectX::XMFLOAT4A v0, v1, v2;
			DirectX::XMVECTOR uv, T;

			v.Position = v0 = DirectX::XMFLOAT4A(float(j), float(heightValues[j + i * mapSize]), float(i), 1.0f);
			v.Normal = DirectX::XMFLOAT4A(1.0f, 1.0f, 1.0f, 0.0f);
			T = DirectX::XMVectorSet(float(j) / float(mapSize - 1), float(i) / float(mapSize - 1), 0, 0);
			uv = DirectX::XMVectorLerpV(uvStart, uvEnd, T);
			DirectX::XMStoreFloat2A(&v.UV, uv);

			vertices.push_back(v);
			vertexMap.insert(std::make_pair(j + i * mapSize, vertices.size() - 1));

			if (vertices.back().Position.y > SNOW_START)
				texture[j + i * mapSize] = colors[0];
			else if (vertices.back().Position.y < WATER_START)
				texture[j + i * mapSize] = colors[1];
			else if (!float4Equal(texture[j + i * mapSize], colors[3]))
				texture[j + i * mapSize] = colors[2];
			
			v.Position = v1 = DirectX::XMFLOAT4A(float(j), float(heightValues[j + (i + 1) * mapSize]), float(i + 1), 1.0f);
			T = DirectX::XMVectorSet(float(j) / float(mapSize - 1), float(i + 1) / float(mapSize - 1), 0, 0);
			uv = DirectX::XMVectorLerpV(uvStart, uvEnd, T);
			DirectX::XMStoreFloat2A(&v.UV, uv);

			vertices.push_back(v);
			vertexMap.insert(std::make_pair(j + (i + 1) * mapSize, vertices.size() - 1));


			if (vertices.back().Position.y > SNOW_START)
				texture[j + (i + 1) * mapSize] = colors[0];
			else if (vertices.back().Position.y < WATER_START)
				texture[j + (i + 1) * mapSize] = colors[1];
			else if (!float4Equal(texture[j + (i + 1) * mapSize], colors[3]))
				texture[j + (i + 1) * mapSize] = colors[2];

			v.Position = v2 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + i * mapSize]), float(i), 1.0f);
			T = DirectX::XMVectorSet(float(j + 1) / float(mapSize - 1), float(i) / float(mapSize - 1), 0, 0);
			uv = DirectX::XMVectorLerpV(uvStart, uvEnd, T);
			DirectX::XMStoreFloat2A(&v.UV, uv);

			vertices.push_back(v);
			vertexMap.insert(std::make_pair((j + 1) + i * mapSize, vertices.size() - 1));

			if (vertices.back().Position.y > SNOW_START)
				texture[(j + 1) + i * mapSize] = colors[0];
			else if (vertices.back().Position.y < WATER_START)
				texture[(j + 1) + i * mapSize] = colors[1];
			else if (!float4Equal(texture[(j + 1) + i * mapSize], colors[3]))
				texture[(j + 1) + i * mapSize] = colors[2];


			DirectX::XMVECTOR e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1), DirectX::XMLoadFloat4A(&v0));
			DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2), DirectX::XMLoadFloat4A(&v0));
			DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));
			DirectX::XMFLOAT4A newNormal;
			DirectX::XMStoreFloat4A(&newNormal, normal);
			triangleNormals.push_back(newNormal);

			float dot = fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, DirectX::XMVectorSet(0, 1, 0, 0))));
			if (dot < 0.5f)
			{
				if (float4Equal(texture[j + i * mapSize], colors[2]))
					texture[j + i * mapSize] = colors[3];
				if (float4Equal(texture[j + (i + 1) * mapSize], colors[2]))
					texture[j + (i + 1) * mapSize] = colors[3];
				if (float4Equal(texture[(j + 1) + i * mapSize], colors[2]))
					texture[(j + 1) + i * mapSize] = colors[3];
			}

			v.Position = v0 = DirectX::XMFLOAT4A(float(j), float(heightValues[j + (i + 1) * mapSize]), float(i + 1), 1.0f);
			T = DirectX::XMVectorSet(float(j) / float(mapSize - 1), float(i + 1) / float(mapSize - 1), 0, 0);
			uv = DirectX::XMVectorLerpV(uvStart, uvEnd, T);
			DirectX::XMStoreFloat2A(&v.UV, uv);
			vertices.push_back(v);
			vertexMap.insert(std::make_pair(j + (i + 1) * mapSize, vertices.size() - 1));


			if (vertices.back().Position.y > SNOW_START)
				texture[j + (i + 1) * mapSize] = colors[0];
			else if (vertices.back().Position.y < WATER_START)
				texture[j + (i + 1) * mapSize] = colors[1];
			else if (!float4Equal(texture[j + (i + 1) * mapSize], colors[3]))
				texture[j + (i + 1) * mapSize] = colors[2];


			v.Position = v1 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + (i + 1) * mapSize]), float(i + 1), 1.0f);
			T = DirectX::XMVectorSet(float(j + 1) / float(mapSize - 1), float(i + 1) / float(mapSize - 1), 0, 0);
			uv = DirectX::XMVectorLerpV(uvStart, uvEnd, T);
			DirectX::XMStoreFloat2A(&v.UV, uv);
			vertices.push_back(v);
			vertexMap.insert(std::make_pair((j + 1) + (i + 1) * mapSize, vertices.size() - 1));

			if (vertices.back().Position.y > SNOW_START)
				texture[(j + 1) + (i + 1) * mapSize] = colors[0];
			else if (vertices.back().Position.y < WATER_START)
				texture[(j + 1) + (i + 1) * mapSize] = colors[1];
			else if (!float4Equal(texture[(j + 1) + (i + 1) * mapSize], colors[3]))
				texture[(j + 1) + (i + 1) * mapSize] = colors[2];

			v.Position = v2 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + i * mapSize]), float(i), 1.0f);
			T = DirectX::XMVectorSet(float(j + 1) / float(mapSize - 1), float(i) / float(mapSize - 1), 0, 0);
			uv = DirectX::XMVectorLerpV(uvStart, uvEnd, T);
			DirectX::XMStoreFloat2A(&v.UV, uv);
			vertices.push_back(v);
			vertexMap.insert(std::make_pair((j + 1) + i * mapSize, vertices.size() - 1));

			if (vertices.back().Position.y > SNOW_START)
				texture[(j + 1) + i * mapSize] = colors[0];
			else if (vertices.back().Position.y < WATER_START)
				texture[(j + 1) + i * mapSize] = colors[1];
			else if (!float4Equal(texture[(j + 1) + i * mapSize], colors[3]))
				texture[(j + 1) + i * mapSize] = colors[2];

			e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1), DirectX::XMLoadFloat4A(&v0));
			e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2), DirectX::XMLoadFloat4A(&v0));
			normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));
			DirectX::XMStoreFloat4A(&newNormal, normal);
			triangleNormals.push_back(newNormal);


			dot = fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, DirectX::XMVectorSet(0, 1, 0, 0))));
			if (dot < 0.5f)
			{
				if (float4Equal(texture[j + (i + 1) * mapSize], colors[2]))
					texture[j + (i + 1) * mapSize] = colors[3];
				if (float4Equal(texture[(j + 1) + (i + 1) * mapSize], colors[2]))
					texture[(j + 1) + (i + 1) * mapSize] = colors[3];
				if (float4Equal(texture[(j + 1) + i * mapSize], colors[2]))
					texture[(j + 1) + i * mapSize] = colors[3];
			}
		}
	}

	for (int y = 0; y < mapSize; y++)
	{
		for (int x = 0; x < mapSize; x++)
		{
			int key = x + y * mapSize;

			auto itStart = vertexMap.lower_bound(key);
			auto itEnd = vertexMap.upper_bound(key);

			auto itWhile = itStart;

			DirectX::XMFLOAT4A normal(0, 0, 0, 0);
			int normalCounter = 0;

			while (itWhile != itEnd)
			{
				if (itWhile->first == key)
				{
					const DirectX::XMFLOAT4A & nor = triangleNormals[itWhile->second / 3];
					normal.x += nor.x;
					normal.y += nor.y;
					normal.z += nor.z;
					normalCounter++;
				}
				itWhile++;
			}
			normal.x /= (float)normalCounter;
			normal.y /= (float)normalCounter;
			normal.z /= (float)normalCounter;

			while (itStart != itEnd)
			{
				if (itStart->first == key)
				{
					vertices[itStart->second].Normal = normal;
				}
				itStart++;
			}
			
		}
	}

	/*for (auto & v : vertices)
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

	}*/

	D3D11_TEXTURE2D_DESC desc = {};
	desc.ArraySize = 1;
	desc.Height = mapSize;
	desc.Width = mapSize;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = texture.data();
	data.SysMemPitch = mapSize * sizeof(DirectX::XMFLOAT4);
	//data.SysMemSlicePitch = 1;

	ID3D11Device * device = Renderer::GetInstance()->GetDevice();

	if (SUCCEEDED(device->CreateTexture2D(&desc, &data, &tex2D)))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		
		HRESULT hr = device->CreateShaderResourceView(tex2D, &srvDesc, &srv);
	}

	return vertices;
}

std::vector<Vertex> TerrainCreator::CreateTerrainFromFloatList2(std::vector<float> heightValues, int mapSize)
{
	std::vector<DirectX::XMFLOAT4A> triangleNormals;
	std::vector<Vertex> vertices;
	std::map<int, Vertex> vertexMap;

	for (int i = 0; i < mapSize; i++)
	{
		for (int j = 0; j < mapSize; j++)
		{

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

	if (index > 0 && index % mapSize != 0)
	{
		counter++;
		normal = xmAdd(normal, normals[(index - 1) / 3]);
	}

	if (index > 3)
	{
		counter++;
		normal = xmAdd(normal, normals[(index - 1) / 3]);
	}

	if (index > 3)
	{
		counter++;
		normal = xmAdd(normal, normals[(index - 1) / 3]);
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

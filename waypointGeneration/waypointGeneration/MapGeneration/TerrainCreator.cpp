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

std::vector<Vertex> TerrainCreator::CreateTerrainFromFloatList(std::vector<float> heightValues, int mapSize, ID3D11ShaderResourceView *& srv, ID3D11Texture2D *& tex2D)
{
	srv = nullptr;
	tex2D = nullptr;
	std::vector<DirectX::XMFLOAT4A> triangleNormals;
	std::vector<Vertex> vertices;

	DirectX::XMFLOAT4 colors[3] = {
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f}
	};

	std::vector<DirectX::XMFLOAT4> texture(mapSize * mapSize);
	
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

			if (vertices.back().Position.y > 5.0f)
				texture[j + i * mapSize] = colors[0];
			else if (vertices.back().Position.y < -5.0f)
				texture[j + i * mapSize] = colors[1];
			else
				texture[j + i * mapSize] = colors[2];


			v.Position = v1 = DirectX::XMFLOAT4A(float(j), float(heightValues[j + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			if (vertices.back().Position.y > 5.0f)
				texture[j + (i + 1) * mapSize] = colors[0];
			else if (vertices.back().Position.y < -5.0f)
				texture[j + (i + 1) * mapSize] = colors[1];
			else
				texture[j + (i + 1) * mapSize] = colors[2];

			v.Position = v2 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + i * mapSize]), float(i), 1.0f);
			vertices.push_back(v);

			if (vertices.back().Position.y > 5.0f)
				texture[(j + 1) + i * mapSize] = colors[0];
			else if (vertices.back().Position.y < -5.0f)
				texture[(j + 1) + i * mapSize] = colors[1];
			else
				texture[(j + 1) + i * mapSize] = colors[2];

			DirectX::XMVECTOR e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1), DirectX::XMLoadFloat4A(&v0));
			DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2), DirectX::XMLoadFloat4A(&v0));
			DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));

			for (int k = vertices.size() - 4; k < vertices.size(); k++)
				DirectX::XMStoreFloat4A(&vertices[k].Normal, normal);

			triangleNormals.push_back(vertices.back().Normal);

			v.Position = v0 = DirectX::XMFLOAT4A(float(j), float(heightValues[j + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			if (vertices.back().Position.y > 5.0f)
				texture[j + (i + 1) * mapSize] = colors[0];
			else if (vertices.back().Position.y < -5.0f)
				texture[j + (i + 1) * mapSize] = colors[1];
			else
				texture[j + (i + 1) * mapSize] = colors[2];

			v.Position = v1 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + (i + 1) * mapSize]), float(i + 1), 1.0f);
			vertices.push_back(v);

			if (vertices.back().Position.y > 5.0f)
				texture[(j + 1) + (i + 1) * mapSize] = colors[0];
			else if (vertices.back().Position.y < -5.0f)
				texture[(j + 1) + (i + 1) * mapSize] = colors[1];
			else
				texture[(j + 1) + (i + 1) * mapSize] = colors[2];

			v.Position = v2 = DirectX::XMFLOAT4A(float(j + 1), float(heightValues[(j + 1) + i * mapSize]), float(i), 1.0f);
			vertices.push_back(v);

			if (vertices.back().Position.y > 5.0f)
				texture[(j + 1) + i * mapSize] = colors[0];
			else if (vertices.back().Position.y < -5.0f)
				texture[(j + 1) + i * mapSize] = colors[1];
			else
				texture[(j + 1) + i * mapSize] = colors[2];

			e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1), DirectX::XMLoadFloat4A(&v0));
			e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2), DirectX::XMLoadFloat4A(&v0));
			normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));

			for (int k = vertices.size() - 4; k < vertices.size(); k++)
				DirectX::XMStoreFloat4A(&vertices[k].Normal, normal);

			triangleNormals.push_back(vertices.back().Normal);
		}
	}

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

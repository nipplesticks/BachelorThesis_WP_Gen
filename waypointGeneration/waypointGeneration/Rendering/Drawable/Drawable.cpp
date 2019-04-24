#include "waypointGenerationPCH.h"
#include "Drawable.h"
#include "../Rendering/Renderer.h"
#include "../Rendering/Passess/IRender.h"
#include "../../Game/QuadTree/QuadTree.h"

// remove
#include <vector>

Drawable::Drawable() : Transform()
{
	m_texture = Renderer::GetInstance()->GetDefaultTexture();
}

Drawable::~Drawable()
{
	if (m_vertexBuffer)
		m_vertexBuffer->Release();

	m_vertexBuffer = nullptr;
}

void Drawable::SetVertices(std::vector<Vertex>* vertices)
{
	m_vertices = vertices;

	if (m_vertexBuffer)
		m_vertexBuffer->Release();
	m_vertexBuffer = nullptr;

	//size_t size = vertices->size();
	//Vertex v = (*vertices)[0];
	//DirectX::XMFLOAT2 min = DirectX::XMFLOAT2(v.Position.x, v.Position.z);
	//DirectX::XMFLOAT2 max = DirectX::XMFLOAT2(v.Position.x, v.Position.z);

	//for (size_t i = 1; i < size; i++)
	//{
	//	Vertex tempV = (*vertices)[i];
	//	DirectX::XMFLOAT2 tempPos = DirectX::XMFLOAT2(tempV.Position.x, tempV.Position.z);
	//	// Mesh min
	//	if (tempPos.x < min.x) min.x = tempPos.x;
	//	if (tempPos.y < min.y) min.y = tempPos.y;
	//	// Mesh max
	//	if (tempPos.x > max.x) max.x = tempPos.x;
	//	if (tempPos.y > max.y) max.y = tempPos.y;
	//}

	//m_min = min;
	//m_max = max;
	//DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&max), DirectX::XMLoadFloat2(&min));
	//DirectX::XMStoreFloat2(&m_size, vec);

	UpdateMinAndMax();

	/* Create Vertex Buffer */
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	m_vertices->shrink_to_fit();
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(Vertex) * (UINT)m_vertices->size();

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_vertices->data();
	if (FAILED(Renderer::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer)))
	{
		exit(-1);
	}
}

void Drawable::SetTexture(ID3D11ShaderResourceView * texture)
{
	m_texture = texture;
}

void Drawable::SetColor(float r, float g, float b, float a)
{
	m_color = DirectX::XMFLOAT4A(r, g, b, a);
}

void Drawable::SetColor(const DirectX::XMFLOAT3 & color)
{
	m_color.x = color.x;
	m_color.y = color.y;
	m_color.z = color.z;
	m_color.w = 1.0f;
}

void Drawable::SetColor(const DirectX::XMFLOAT4 & color)
{
	m_color.x = color.x;
	m_color.y = color.y;
	m_color.z = color.z;
	m_color.w = color.w;
}

const DirectX::XMFLOAT2A & Drawable::GetUVOffset() const
{
	return m_uvOffset;
}

void Drawable::SetUVOffset(float x, float y)
{
	m_uvOffset.x = x;
	m_uvOffset.y = y;
}

D3D11_PRIMITIVE_TOPOLOGY Drawable::GetTopology() const
{
	return m_topology;
}

void Drawable::SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	m_topology = topology;
}

const DirectX::XMFLOAT4A & Drawable::GetColor() const
{
	return m_color;
}

const std::vector<Vertex>* Drawable::GetVertices() const
{
	return m_vertices;
}

ID3D11Buffer * Drawable::GetBuffer() const
{
	return m_vertexBuffer;
}

const DirectX::XMFLOAT2 & Drawable::GetSize() const
{
	return m_size;
}

const DirectX::BoundingBox & Drawable::GetBoundingBox() const
{
	return m_boundingBox;
}

void Drawable::SetPickable(bool isPickable)
{
	m_isPickable = isPickable;
}

bool Drawable::IsPickable() const
{
	return m_isPickable;
}

void Drawable::SetPath(const std::vector<DirectX::XMFLOAT2>& path)
{
	m_path = path;
}

std::vector<DirectX::XMFLOAT2>& Drawable::GetPath()
{
	return m_path;
}

void Drawable::SetSpeed(float spd)
{
	m_speed = spd;
}

float Drawable::GetSpeed() const
{
	return m_speed;
}

void Drawable::UnitUpdate(double dt, QuadTree * qt)
{
	using namespace DirectX;
	if (!m_path.empty())
	{
		XMFLOAT3 pos = GetPosition();
		XMFLOAT2 current(pos.x, pos.z);
		XMFLOAT2 target = m_path.front();
		XMFLOAT2 dummy;

		Triangle * tri = nullptr;
		while (qt && m_path.size() > 1 && tri == nullptr)
		{
			XMFLOAT2 nexTarget = m_path[1];
			tri = qt->LineIntersectionTriangle(current, nexTarget, true, dummy);
			if (tri == nullptr)
			{
				m_path.erase(m_path.begin());
				target = nexTarget;
			}
		}


		XMVECTOR vCurrent = XMLoadFloat2(&current);
		XMVECTOR vTarget = XMLoadFloat2(&target);
		XMVECTOR vLine = XMVectorSubtract(vTarget, vCurrent);
		float lBefore = XMVectorGetX(XMVector2Length(vLine));
		XMVECTOR vDir = XMVector2Normalize(vLine);
		vCurrent = XMVectorAdd(vCurrent, XMVectorScale(vDir, m_speed * dt));
		vLine = XMVectorSubtract(vTarget, vCurrent);
		float lAfter = XMVectorGetX(XMVector2Length(vLine));

		if (lAfter < lBefore)
			XMStoreFloat2(&current, vCurrent);
		else
		{
			XMStoreFloat2(&current, vTarget);
			m_path.erase(m_path.begin());
		}
		pos.x = current.x;
		pos.z = current.y;

		SetPosition(pos);
	}
}

ID3D11ShaderResourceView * Drawable::GetTexture() const
{
	return m_texture;
}

void Drawable::UseDepthBuffer(bool val)
{
	m_useDepth = val;
}

void Drawable::UpdateMinAndMax()
{
	size_t size = m_vertices->size();
	Vertex v = (*m_vertices)[0];
	DirectX::XMFLOAT2 min = DirectX::XMFLOAT2(v.Position.x, v.Position.z);
	DirectX::XMFLOAT2 max = DirectX::XMFLOAT2(v.Position.x, v.Position.z);

	for (size_t i = 1; i < size; i++)
	{
		Vertex tempV = (*m_vertices)[i];
		DirectX::XMFLOAT2 tempPos = DirectX::XMFLOAT2(tempV.Position.x, tempV.Position.z);
		// Mesh min
		if (tempPos.x < min.x) min.x = tempPos.x;
		if (tempPos.y < min.y) min.y = tempPos.y;
		// Mesh max
		if (tempPos.x > max.x) max.x = tempPos.x;
		if (tempPos.y > max.y) max.y = tempPos.y;
	}

	m_min = min;
	m_max = max;

	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&max), DirectX::XMLoadFloat2(&min));
	DirectX::XMStoreFloat2(&m_size, vec);
}

void Drawable::Draw()
{
	if (m_isActive)
	{
		if (!m_useDepth)
			IRender::QueueNoDepth(this);
		else if (m_color.w > 0.999f)
			IRender::Queue(this);
		else
			IRender::QueueAlpha(this);
	}
}

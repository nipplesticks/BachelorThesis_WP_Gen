#include "waypointGenerationPCH.h"
#include "Drawable.h"
#include "../Rendering/Renderer.h"
#include "../Rendering/Passess/IRender.h"

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

	// Calculate mesh size
	// TODO

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

ID3D11ShaderResourceView * Drawable::GetTexture() const
{
	return m_texture;
}

void Drawable::Draw()
{
	if (m_color.w > 0.999f)
		IRender::Queue(this);
	else
		IRender::QueueAlpha(this);
}

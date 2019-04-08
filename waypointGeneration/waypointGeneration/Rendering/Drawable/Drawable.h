#pragma once
#include "Transform/Transform.h"
#include "../structs.h"

class Drawable : public Transform
{
public:
	Drawable();
	~Drawable();

	void SetVertices(std::vector<Vertex> * vertices);
	void SetTexture(ID3D11ShaderResourceView * texture);
	void SetColor(float r, float g, float b, float a = 1.0f);
	void SetColor(const DirectX::XMFLOAT3 &color);
	void SetColor(const DirectX::XMFLOAT4 &color);

	const DirectX::XMFLOAT2A & GetUVOffset() const;
	void SetUVOffset(float x, float y);

	D3D11_PRIMITIVE_TOPOLOGY GetTopology() const;
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

	const DirectX::XMFLOAT4A & GetColor() const;
	const std::vector<Vertex> * GetVertices() const;
	ID3D11Buffer * GetBuffer() const;

	const DirectX::XMFLOAT2 & GetSize() const;
	const DirectX::BoundingBox & GetBoundingBox() const;

	void SetPickable(bool isPickable);
	bool IsPickable() const;

	ID3D11ShaderResourceView * GetTexture() const;


	void Draw();

private:
	DirectX::XMFLOAT4A m_color = {1,1,1,1};
	DirectX::XMFLOAT2A m_uvOffset = { 0,0 };

	bool m_isPickable = false;

	D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<Vertex> * m_vertices = nullptr;

	DirectX::XMFLOAT2 m_size;
	DirectX::BoundingBox m_boundingBox;

	// D3D11 stuff
	ID3D11Buffer * m_vertexBuffer = nullptr;
	ID3D11ShaderResourceView * m_texture = nullptr;
};
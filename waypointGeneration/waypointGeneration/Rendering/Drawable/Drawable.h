#pragma once
#include "Transform/Transform.h"
#include "../structs.h"

class QuadTree;

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

	void SetPath(const std::vector<DirectX::XMFLOAT2> & path);
	std::vector<DirectX::XMFLOAT2> & GetPath();

	void SetSpeed(float spd);
	float GetSpeed() const;

	void UnitUpdate(double dt, QuadTree * qt);

	ID3D11ShaderResourceView * GetTexture() const;

	void UseDepthBuffer(bool val);

	void UpdateMinAndMax();

	void SetActive(bool val) { m_isActive = val; }
	bool IsActive() const { return m_isActive; }

	void Draw();

private:
	std::vector<DirectX::XMFLOAT2> m_path;
	float m_speed = 10.0f;

	DirectX::XMFLOAT4A m_color = {1,1,1,1};
	DirectX::XMFLOAT2A m_uvOffset = { 0,0 };

	bool m_isPickable = false;

	bool m_useDepth = true;

	bool m_isActive = true;

	D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::vector<Vertex> * m_vertices = nullptr;

	DirectX::XMFLOAT2 m_size;
	DirectX::BoundingBox m_boundingBox;
	DirectX::XMFLOAT2 m_min = { FLT_MAX, FLT_MAX };
	DirectX::XMFLOAT2 m_max = { FLT_MIN, FLT_MIN };

	// D3D11 stuff
	ID3D11Buffer * m_vertexBuffer = nullptr;
	ID3D11ShaderResourceView * m_texture = nullptr;
};

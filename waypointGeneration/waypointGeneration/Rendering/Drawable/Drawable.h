#pragma once
#include "Transform/Transform.h"
#include "../structs.h"

class Drawable : public Transform
{
public:
	Drawable();
	~Drawable();

	void SetVertices(std::vector<Vertex> * vertices);
	void SetColor(float r, float g, float b);
	void SetColor(const DirectX::XMFLOAT3 &color);

	const DirectX::XMFLOAT4A & GetColor() const;
	const std::vector<Vertex> * GetVertices() const;
	ID3D11Buffer * GetBuffer() const;

	void Draw();

private:
	DirectX::XMFLOAT4A m_color;

	std::vector<Vertex> * m_vertices = nullptr;


	// D3D11 stuff
	ID3D11Buffer * m_vertexBuffer = nullptr;
};
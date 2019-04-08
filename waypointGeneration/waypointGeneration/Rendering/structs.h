#pragma once
#include <DirectXMath.h>
#include <DirectXCollision.h>
class Drawable;

struct CameraBuffer
{
	DirectX::XMFLOAT4X4A viewProjection;
	DirectX::XMFLOAT4A cameraPosition;
};

struct ObjectBuffer
{
	DirectX::XMFLOAT4X4A worldMatrix;
	DirectX::XMFLOAT4A color;
	DirectX::XMFLOAT2A uvOffset;
};

struct MouseBuffer
{
	DirectX::XMUINT4 MousePos;
};

struct Triangle
{
	Triangle(const DirectX::XMFLOAT3 & p0,
		const DirectX::XMFLOAT3 & p1,
		const DirectX::XMFLOAT3 & p2)
	{
		points[0] = p0;
		points[1] = p1;
		points[2] = p2;

		DirectX::XMVECTOR e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));
		DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p0));
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));

		DirectX::XMStoreFloat3(&normal, n);
	}
	Triangle(const DirectX::XMFLOAT4A & p0,
		const DirectX::XMFLOAT4A & p1,
		const DirectX::XMFLOAT4A & p2)
	{
		points[0] = DirectX::XMFLOAT3(p0.x, p0.y, p0.z);
		points[1] = DirectX::XMFLOAT3(p1.x, p1.y, p1.z);
		points[2] = DirectX::XMFLOAT3(p2.x, p2.y, p2.z);

		DirectX::XMVECTOR e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&p1), DirectX::XMLoadFloat4A(&p0));
		DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&p2), DirectX::XMLoadFloat4A(&p0));
		DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));

		DirectX::XMStoreFloat3(&normal, n);
	}

	Drawable * owner;
	DirectX::XMFLOAT3 points[3];
	DirectX::XMFLOAT3 normal;
};

struct Vertex
{
	DirectX::XMFLOAT4A Position;
	DirectX::XMFLOAT4A Normal;
	DirectX::XMFLOAT2A UV;

	bool operator==(const Vertex & v)
	{
		if (fabs(Position.x - v.Position.x) > 0.001f)
			return false;
		if (fabs(Position.y - v.Position.y) > 0.001f)
			return false;
		if (fabs(Position.z - v.Position.z) > 0.001f)
			return false;

		return true;
	}
};
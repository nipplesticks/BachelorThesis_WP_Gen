#pragma once
#include <DirectXMath.h>

struct CameraBuffer
{
	DirectX::XMFLOAT4X4A viewProjection;
	DirectX::XMFLOAT4A cameraPosition;
};

struct ObjectBuffer
{
	DirectX::XMFLOAT4X4A worldMatrix;
	DirectX::XMFLOAT4A color;
};

struct Vertex
{
	DirectX::XMFLOAT4A Position;
	DirectX::XMFLOAT4A Normal;


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
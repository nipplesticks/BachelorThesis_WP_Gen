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
};
#pragma once
#include <DirectXMath.h>
class Transform
{
public:
	Transform();
	~Transform();

	void Update();

	void SetPosition(const DirectX::XMFLOAT3 & position);
	void SetPosition(float x, float y, float z);
	void Translate(const DirectX::XMFLOAT3 & translation);
	void Translate(float x, float y, float z);

	void SetRotation(const DirectX::XMFLOAT3 & rotation);
	void SetRotation(float x, float y, float z);
	void SetRotation2(const DirectX::XMFLOAT3 & rotation, float angle);
	void Rotate(const DirectX::XMFLOAT3 & rotation);
	void Rotate(float x, float y, float z);

	void SetScale(const DirectX::XMFLOAT3 & scale);
	void SetScale(float x, float y, float z);
	void Scale(const DirectX::XMFLOAT3 & scale);
	void Scale(float x, float y, float z);

	const DirectX::XMFLOAT3 & GetPosition() const;
	const DirectX::XMFLOAT3 & GetRotation() const;
	const DirectX::XMFLOAT3 & GetScale() const;

	const DirectX::XMFLOAT4X4A & GetWorldMatrix() const;
private:
	DirectX::XMFLOAT3 m_position = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 m_scale = DirectX::XMFLOAT3(1, 1, 1);
	DirectX::XMFLOAT3 m_rotation = DirectX::XMFLOAT3(0, 0, 0);
	
	float m_angle = 0.0f;
	bool m_useForcedRot = false;


	DirectX::XMFLOAT4X4A m_worldMatrix;
};
#pragma once
#include "../../Rendering/Drawable/Drawable.h"
#include "../Pathfinding/Waypoint.h"
#include "../../Rendering/structs.h"
#include <DirectXCollision.h>
#include <vector>

class Quadrant
{
public:
	Quadrant();
	~Quadrant();

	void Create(const DirectX::XMFLOAT2 & pos, float size, UINT currentLevel, bool isLeaf);
	void Create(float startX, float startY, float size, unsigned int currentLevel, bool isLeaf);

	void AddChild(int index);

	void SetParent(int index);

	const unsigned int * GetChildren() const;

	int GetParent() const;

	unsigned int GetNrOfChildren() const;

	bool Intersects(const DirectX::BoundingOrientedBox & OBB) const;
	bool Intersects(const DirectX::BoundingBox & AABB) const;
	bool Intersects(const DirectX::XMFLOAT2 & ray, const DirectX::XMFLOAT2 & origin, float & t) const;
	bool Intersects(const DirectX::XMFLOAT2 & point) const;
	bool Intersects(const Triangle & tri);
	bool Intersects(const DirectX::BoundingSphere & BS) const;

	bool Intersects(const DirectX::XMVECTOR & rayOrigin, const DirectX::XMVECTOR & rayDirection);
	bool Intersects(const DirectX::XMVECTOR & point);

	const DirectX::XMFLOAT2 & GetMin() const;

	const DirectX::XMFLOAT2 & GetMax() const;

	const unsigned int & GetLevel() const;

	bool IsLeaf() const;

	const float & GetSize() const;

	bool ContainsDrawables() const;
	bool ContainsWaypoints() const;
	bool ContainsTriangles() const;

	void SetObject(Drawable * object);
	void SetObject(Waypoint * wp);
	void SetObject(Triangle * t);

	void ClearAllObjects();
	void ClearTriangles();
	void ClearDrawables();
	void ClearWaypoints();

	const std::vector<Drawable*> & GetDrawables() const;
	const std::vector<Waypoint*> & GetWaypoints() const;
	const std::vector<Triangle*> & GetTriangles() const;

private:
	DirectX::BoundingBox m_aabb;
	DirectX::XMFLOAT2 m_min, m_max;

	long int m_parent = -1;
	bool m_isLeaf = true;
	UINT m_children[4] = { 0 };
	UINT m_nrOfChildren = 0;
	UINT m_level = 0;
	float m_size = 0.0f;

	std::vector<Drawable*> m_containingDrawables;
	std::vector<Waypoint*> m_containingWaypoints;
	std::vector<Triangle*> m_containingTriangles;
};
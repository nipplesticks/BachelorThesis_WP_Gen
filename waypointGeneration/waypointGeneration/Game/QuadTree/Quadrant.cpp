#include "waypointGenerationPCH.h"
#include "Quadrant.h"

Quadrant::Quadrant()
{
	
}

Quadrant::~Quadrant()
{

}

void Quadrant::Create(const DirectX::XMFLOAT2 & pos, float size, UINT currentLevel, bool isLeaf)
{
	m_isLeaf = isLeaf;
	m_level = currentLevel;

	m_min = pos;

	DirectX::XMFLOAT2 xmP2 = pos;
	xmP2.x += size;
	xmP2.y += size;

	m_max = xmP2;
	DirectX::XMVECTOR p1 = DirectX::XMLoadFloat2(&pos);
	DirectX::XMVECTOR p2 = DirectX::XMLoadFloat2(&xmP2);

	m_aabb.CreateFromPoints(m_aabb, p1, p2);
}

void Quadrant::Create(float startX, float startY, float size, unsigned int currentLevel, bool isLeaf)
{
	Create({ startX, startY }, size, currentLevel, isLeaf);
}

void Quadrant::AddChild(int index)
{
	m_children[m_nrOfChildren++] = index;
}

void Quadrant::SetParent(int index)
{
	m_parent = index;
}

const unsigned int * Quadrant::GetChildren() const
{
	return m_children;
}

int Quadrant::GetParent() const
{
	return m_parent;
}

unsigned int Quadrant::GetNrOfChildren() const
{
	return m_nrOfChildren;
}

bool Quadrant::Intersects(const DirectX::BoundingOrientedBox & OBB)
{
	return m_aabb.Intersects(OBB);
}

bool Quadrant::Intersects(const DirectX::BoundingBox & AABB)
{
	return m_aabb.Intersects(AABB);
}

bool Quadrant::Intersects(const DirectX::XMFLOAT2 & ray, const DirectX::XMFLOAT2 & origin, float & t)
{
	return m_aabb.Intersects(DirectX::XMLoadFloat2(&origin), DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&ray)), t);
}

bool Quadrant::Intersects(const DirectX::XMFLOAT2 & point)
{
	return m_aabb.Contains(DirectX::XMLoadFloat2(&point));
}

bool Quadrant::Intersects(const Triangle & tri)
{
	Triangle t = tri;
	DirectX::XMVECTOR p[3];

	for (int i = 0; i < 3; i++)
		p[i] = DirectX::XMLoadFloat2(&DirectX::XMFLOAT2(t.points[i].x, t.points[i].z));

	return m_aabb.Intersects(p[0], p[1], p[2]);
}

const DirectX::XMFLOAT2 & Quadrant::GetMin() const
{
	return m_min;
}

const DirectX::XMFLOAT2 & Quadrant::GetMax() const
{
	return m_max;
}

const unsigned int & Quadrant::GetLevel() const
{
	return m_level;
}

bool Quadrant::IsLeaf() const
{
	return m_isLeaf;
}

bool Quadrant::ContainsDrawables() const
{
	return !m_containingDrawables.empty();
}

bool Quadrant::ContainsWaypoints() const
{
	return !m_containingWaypoints.empty();
}

bool Quadrant::ContainsTriangles() const
{
	return !m_containingTriangles.empty();
}

void Quadrant::SetObject(Drawable * object)
{
	m_containingDrawables.push_back(object);
}

void Quadrant::SetObject(Waypoint * wp)
{
	m_containingWaypoints.push_back(wp);
}

void Quadrant::SetObject(Triangle * t)
{
	m_containingTriangles.push_back(t);
}

void Quadrant::ClearAllObjects()
{
	m_containingDrawables.clear();
	m_containingWaypoints.clear();
	m_containingTriangles.clear();
}

void Quadrant::ClearTriangles()
{
	m_containingTriangles.clear();
}

void Quadrant::ClearDrawables()
{
	m_containingDrawables.clear();
}

void Quadrant::ClearWaypoints()
{
	m_containingWaypoints.clear();
}

const std::vector<Drawable*>& Quadrant::GetDrawables() const
{
	return m_containingDrawables;
}

const std::vector<Waypoint*>& Quadrant::GetWaypoints() const
{
	return m_containingWaypoints;
}

const std::vector<Triangle*>& Quadrant::GetTriangles() const
{
	return m_containingTriangles;
}
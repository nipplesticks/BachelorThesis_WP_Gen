#include "waypointGenerationPCH.h"
#include "QuadTree.h"

QuadTree::QuadTree()
{
	
}

QuadTree::~QuadTree()
{
}

void QuadTree::BuildTree(unsigned int maxLevel, unsigned int worldSize, const DirectX::XMFLOAT2 & worldStart)
{
	m_quadTree.clear();
	m_leafs.clear();
	m_maximumLevel = maxLevel;
	m_worldSize = worldSize;

	size_t numberOfQuadrants = 0;

	for (unsigned int level = 0; level <= maxLevel; level++)
		numberOfQuadrants += (unsigned int)std::pow(4, level);

	m_quadTree = std::vector<Quadrant>(numberOfQuadrants);

	float size = (float)worldSize;

	Quadrant q;
	q.Create(worldStart, size, 0, false);
	size_t index = 0;
	m_quadTree[index++] = q;

	for (unsigned int level = 1; level <= maxLevel; level++)
	{
		bool isLeaf = level == maxLevel;
		float rowCol = (float)std::pow(2, level);
		size = size * 0.5f;

		for (unsigned int y = 0; y < rowCol; y++)
		{
			DirectX::XMFLOAT2 currentPos = worldStart;
			currentPos.y += size * y;

			for (unsigned int x = 0; x < rowCol; x++)
			{
				currentPos.x = worldStart.x + size * x;
				q.Create(currentPos, size, level, isLeaf);
				size_t parent = _GetQuadrantIndex(currentPos, level - 1);
				m_quadTree[index] = q;
				if (isLeaf)
					m_leafs.push_back(&m_quadTree[index]);
				m_quadTree[index].SetParent((int)parent);
				m_quadTree[parent].AddChild((int)index++);
			}
		}
	}
}

void QuadTree::BuildTree(unsigned int maxLevel, unsigned int worldSize, float worldStartX, float worldStartY)
{
	BuildTree(maxLevel, worldSize, DirectX::XMFLOAT2(worldStartX, worldStartY));
}

void QuadTree::PlaceObjects(std::vector<Drawable*>& objectVector)
{
	for (auto & q : m_leafs)
		q->ClearDrawables();

	int size = (int)objectVector.size();

	for (size_t i = 0; i < size; i++)
		_traverseAndPlace(objectVector[i], 0);
}

void QuadTree::PlaceObjects(std::vector<Waypoint*>& objectVector)
{
	for (auto & q : m_leafs)
		q->ClearWaypoints();

	int size = (int)objectVector.size();

	for (size_t i = 0; i < size; i++)
		_traverseAndPlace(objectVector[i], 0);
}

void QuadTree::PlaceObjects(std::vector<Triangle*>& objectVector)
{
	for (auto & q : m_leafs)
		q->ClearTriangles();

	int size = (int)objectVector.size();

	for (size_t i = 0; i < size; i++)
		_traverseAndPlace(objectVector[i], 0);
}

unsigned int QuadTree::GetWorldSize() const
{
	return m_worldSize;
}

unsigned int QuadTree::GetMaxTreeLevel() const
{
	return m_maximumLevel;
}

Triangle * QuadTree::LineIntersectionTriangle(const DirectX::XMFLOAT2 & lineStart, const DirectX::XMFLOAT2 & lineEnd, bool firstHitFound, DirectX::XMFLOAT2 & interSectionPoint)
{
	DirectX::XMVECTOR lStart = DirectX::XMLoadFloat2(&lineStart);
	DirectX::XMVECTOR lEnd = DirectX::XMLoadFloat2(&lineEnd);
	interSectionPoint = DirectX::XMFLOAT2(FLT_MAX, FLT_MAX);

	float t = FLT_MAX;
	Triangle * tri = nullptr;
	

	_triangleTraversalLine(tri, interSectionPoint, lStart, lEnd, DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(lEnd, lStart)), firstHitFound, 0, t);

	return tri;
}

Triangle * QuadTree::RayIntersectionTriangle3D(const DirectX::XMFLOAT3 & rayOrigin, const DirectX::XMFLOAT3 & rayDir, bool firstHitFound, DirectX::XMFLOAT3 & interSectionPoint)
{
	DirectX::XMVECTOR origin = DirectX::XMLoadFloat3(&rayOrigin);
	DirectX::XMFLOAT2 o2D = { rayOrigin.x, rayOrigin.z };
	DirectX::XMVECTOR origin2D = DirectX::XMLoadFloat2(&o2D);

	DirectX::XMVECTOR dir = DirectX::XMVector2Normalize(DirectX::XMLoadFloat3(&rayDir));

	DirectX::XMFLOAT2 d2D = { rayDir.x, rayDir.z };
	DirectX::XMVECTOR dir2D = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&d2D));

	interSectionPoint = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	float t = FLT_MAX;
	Triangle * tri = nullptr;

	_triangleTraversalRay3D(tri, interSectionPoint, origin, origin2D, dir, dir2D, firstHitFound, 0, t);

	return tri;
}

Triangle * QuadTree::PointInsideTriangle(const DirectX::XMFLOAT2 & point, bool firstHitFound)
{
	DirectX::XMVECTOR p = DirectX::XMLoadFloat2(&point);
	Triangle * tri = nullptr;

	_triangleTraversePoint(tri, p, 0);

	return tri;
}

void QuadTree::_triangleTraversalLine(Triangle *& outPtr, DirectX::XMFLOAT2 & interSectionPoint, const DirectX::XMVECTOR & lineStart, const DirectX::XMVECTOR & lineEnd, const DirectX::XMVECTOR & dir, bool firstHitFound, int quadIndex, float & t)
{
	if (((firstHitFound && outPtr == nullptr) || !firstHitFound) && t > 0.0f)
	{
		float tempT = 0.0f;
		if (m_quadTree[quadIndex].Intersects(lineStart, dir))
		{
			int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

			if (nrOfChildren > 0)
			{
				const unsigned int * children = m_quadTree[quadIndex].GetChildren();
				for (int i = 0; i < nrOfChildren; i++)
					_triangleTraversalLine(outPtr, interSectionPoint, lineStart, lineEnd, dir, firstHitFound, children[i], t);
			}
			else
			{
				const std::vector<Triangle*> & triangles = m_quadTree[quadIndex].GetTriangles();
				size_t size = triangles.size();

				float tTemp = FLT_MAX;
				DirectX::XMFLOAT2 interSectionPointTemp;

				bool canContinue = true;

				for (int i = 0; i < size && canContinue; i++)
				{
					if (_lineTriangleIntersection(triangles[i], interSectionPointTemp, lineStart, lineEnd, tTemp) && tTemp < t)
					{
						outPtr = triangles[i];
						t = tTemp;
						interSectionPoint = interSectionPointTemp;
						canContinue = !firstHitFound;
					}
				}
			}
		}
	}
}

void QuadTree::_triangleTraversalRay3D(Triangle *& outPtr, DirectX::XMFLOAT3 & interSectionPoint, const DirectX::XMVECTOR & rayOrigin, const DirectX::XMVECTOR & rayOrigin2D, const DirectX::XMVECTOR & ray, const DirectX::XMVECTOR & ray2D, bool firstHitFound, int quadIndex, float & t)
{
	if (((firstHitFound && outPtr == nullptr) || !firstHitFound) && t > 0.0f)
	{
		float tempT = 0.0f;
		if (m_quadTree[quadIndex].Intersects(rayOrigin2D) || m_quadTree[quadIndex].Intersects(rayOrigin2D, ray2D))
		{
			int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

			if (nrOfChildren > 0)
			{
				const unsigned int * children = m_quadTree[quadIndex].GetChildren();
				for (int i = 0; i < nrOfChildren; i++)
					_triangleTraversalRay3D(outPtr, interSectionPoint, rayOrigin, rayOrigin2D, ray, ray2D, firstHitFound, children[i], t);
			}
			else
			{
				const std::vector<Triangle*> & triangles = m_quadTree[quadIndex].GetTriangles();
				size_t size = triangles.size();

				float tTemp = FLT_MAX;

				bool canContinue = true;

				for (int i = 0; i < size && canContinue; i++)
				{
					if (_Ray3DTriangleIntersection(triangles[i], rayOrigin, ray, tTemp) && tTemp < t)
					{
						outPtr = triangles[i];
						t = tTemp;

						
						DirectX::XMVECTOR ip = DirectX::XMVectorAdd(rayOrigin, DirectX::XMVectorScale(ray, t));

						DirectX::XMStoreFloat3(&interSectionPoint, ip);
						
						canContinue = !firstHitFound;
					}
				}
			}
		}
	}
}

void QuadTree::_triangleTraversePoint(Triangle *& tPtr, const DirectX::XMVECTOR & p, int quadIndex)
{
	if (tPtr == nullptr)
	{
		if (m_quadTree[quadIndex].Intersects(p))
		{
			int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

			if (nrOfChildren > 0)
			{
				const unsigned int * children = m_quadTree[quadIndex].GetChildren();
				for (int i = 0; i < nrOfChildren; i++)
					_triangleTraversePoint(tPtr, p, children[i]);
			}
			else
			{
				const std::vector<Triangle*> & triangles = m_quadTree[quadIndex].GetTriangles();
				size_t size = triangles.size();
				bool canContinue = true;

				for (int i = 0; i < size && canContinue; i++)
				{
					if (_PointTriangleIntersection(triangles[i], p))
					{
						tPtr = triangles[i];
						canContinue = false;
					}
				}
			}
		}
	}
}

bool QuadTree::_lineTriangleIntersection(const Triangle * tri, DirectX::XMFLOAT2 & interSectionPoint, const DirectX::XMVECTOR & lineStart, const DirectX::XMVECTOR & lineEnd, float & t)
{
	static const float EPSILON = 0.0001f;
	using namespace DirectX;
	t = FLT_MAX;
	bool hit = false;

	DirectX::XMFLOAT2 l1S, l1E;
	DirectX::XMStoreFloat2(&l1S, lineStart);
	DirectX::XMStoreFloat2(&l1E, lineEnd);

	DirectX::XMVECTOR isPoint;
	float lSq = FLT_MAX;

	DirectX::XMFLOAT2 l2S, l2E;
	for (int i = 0; i < 3; i++)
	{
		int oIndex = (i + 1) % 3;
		l2S.x = tri->points[i].x;
		l2S.y = tri->points[i].z;
		l2E.x = tri->points[oIndex].x;
		l2E.y = tri->points[oIndex].z;

		XMFLOAT2 b(l1E.x - l1S.x, l1E.y - l1S.y);
		XMFLOAT2 d(l2E.x - l2S.x, l2E.y - l2S.y);

		float bDotDPerp = b.x * d.y - b.y * d.x;

		if (fabs(bDotDPerp) < EPSILON)
			continue;

		XMFLOAT2 c(l2S.x - l1S.x, l2S.y - l1S.y);

		float tTemp = (c.x * d.y - c.y * d.x) / bDotDPerp;

		if (tTemp < 0.0f || tTemp > 1.0f)
			continue;

		float u = (c.x * b.y - c.y * b.x) / bDotDPerp;

		if (u < 0.0f || u > 1.0f)
			continue;

		hit = true;

		if (tTemp < t)
		{
			t = tTemp;
			XMStoreFloat2(&interSectionPoint, XMVectorAdd(lineStart, XMVectorScale(XMLoadFloat2(&b), t)));
			return true;
		}

	}

	return hit;
}

bool QuadTree::_Ray3DTriangleIntersection(const Triangle * tri, const DirectX::XMVECTOR & origin, const DirectX::XMVECTOR & dir, float & t)
{
	return DirectX::TriangleTests::Intersects(origin, dir,
		DirectX::XMLoadFloat3(&tri->points[0]), DirectX::XMLoadFloat3(&tri->points[1]), DirectX::XMLoadFloat3(&tri->points[2]),
		t);
}



bool QuadTree::_PointTriangleIntersection(const Triangle * tri, const DirectX::XMVECTOR & point)
{
	DirectX::XMFLOAT2 s, a, b, c;
	DirectX::XMStoreFloat2(&s, point);
	a.x = tri->points[0].x;
	a.y = tri->points[0].z;
	b.x = tri->points[1].x;
	b.y = tri->points[1].z;
	c.x = tri->points[2].x;
	c.y = tri->points[2].z;

	float as_x = s.x - a.x;
	float as_y = s.y - a.y;

	bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0;

	if ((c.x - a.x) * as_y - (c.y - a.y) * as_x >= 0 == s_ab) return false;

	if ((c.x - b.x)*(s.y - b.y) - (c.y - b.y)*(s.x - b.x) > 0 != s_ab) return false;

	return true;
}

size_t QuadTree::_GetQuadrantIndex(const DirectX::XMFLOAT2 & worldPos, unsigned int level)
{
	level = level > m_maximumLevel ? m_maximumLevel : level;

	size_t levelStartIndex = 0;
	size_t levelEndIndex = 0;

	for (unsigned int i = 0; i < level; i++)
		levelStartIndex += (unsigned int)std::pow(4, i);

	levelEndIndex = levelStartIndex + (unsigned int)std::pow(4, level);

	float size = m_quadTree[levelStartIndex].GetSize();
	DirectX::XMFLOAT2 searchPos = worldPos;

	DirectX::XMFLOAT2 worldStart = m_quadTree[0].GetMin();
	DirectX::XMFLOAT2 worldEnd = m_quadTree[0].GetMax();

	searchPos.x = searchPos.x < worldStart.x ? worldStart.x : searchPos.x;
	searchPos.y = searchPos.y < worldStart.y ? worldStart.y : searchPos.y;
	searchPos.x = searchPos.x > worldEnd.x ? worldEnd.x : searchPos.x;
	searchPos.y = searchPos.y > worldEnd.y ? worldEnd.y : searchPos.y;

	DirectX::XMStoreFloat2(&searchPos, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&searchPos), DirectX::XMLoadFloat2(&worldStart)));
	DirectX::XMFLOAT2 step;
	DirectX::XMStoreFloat2(&step, DirectX::XMVectorDivide(DirectX::XMLoadFloat2(&searchPos), DirectX::XMLoadFloat2(&DirectX::XMFLOAT2(size, size))));

	return levelStartIndex + (int)step.x + (int)step.y * (int)std::pow(2, level);
}

void QuadTree::_traverseAndPlace(Drawable * e, int quadIndex)
{
	DirectX::XMFLOAT3 pos = e->GetPosition();
	if (_insideAABB(DirectX::XMFLOAT2(pos.x, pos.z), e->GetSize(), m_quadTree[quadIndex]))
	{
		int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

		if (nrOfChildren > 0)
		{
			const unsigned int * children = m_quadTree[quadIndex].GetChildren();
			for (int i = 0; i < nrOfChildren; i++)
				_traverseAndPlace(e, children[i]);
		}
		else
			m_quadTree[quadIndex].SetObject(e);
	}
}

void QuadTree::_traverseAndPlace(Waypoint * e, int quadIndex)
{
	if (m_quadTree[quadIndex].Intersects(e->GetPosition()))
	{
		int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

		if (nrOfChildren > 0)
		{
			const unsigned int * children = m_quadTree[quadIndex].GetChildren();
			for (int i = 0; i < nrOfChildren; i++)
				_traverseAndPlace(e, children[i]);
		}
		else
			m_quadTree[quadIndex].SetObject(e);
	}
}

void QuadTree::_traverseAndPlace(Triangle * e, int quadIndex)
{
	if (m_quadTree[quadIndex].Intersects(*e))
	{
		int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

		if (nrOfChildren > 0)
		{
			const unsigned int * children = m_quadTree[quadIndex].GetChildren();
			for (int i = 0; i < nrOfChildren; i++)
				_traverseAndPlace(e, children[i]);
		}
		else
			m_quadTree[quadIndex].SetObject(e);
	}
}

void QuadTree::_pointTraverse(const DirectX::XMFLOAT2 & point, int quadIndex, Drawable *& ePtr) const
{
	if (ePtr == nullptr)
	{
		if (_insideAABB(point, m_quadTree[quadIndex]))
		{
			int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

			if (nrOfChildren > 0)
			{
				const unsigned int * children = m_quadTree[quadIndex].GetChildren();
				for (int i = 0; i < nrOfChildren; i++)
					_pointTraverse(point, children[i], ePtr);
			}
			else
			{
				// Trace Objects
				const std::vector<Drawable*> & objects = m_quadTree[quadIndex].GetDrawables();

				size_t size = objects.size();

				for (int i = 0; i < size; i++)
				{
					DirectX::BoundingBox bb = objects[i]->GetBoundingBox();

					DirectX::XMVECTOR pointVec = DirectX::XMLoadFloat2(&point);
					if (bb.Contains(pointVec))
					{
						ePtr = objects[i];
					}
				}
			}
		}
	}
}

void QuadTree::_traverseWithRay(const DirectX::XMFLOAT2 & rayOrigin, const DirectX::XMFLOAT2 & rayDirection, int quadIndex, float & t, Drawable *& ePtr, Drawable * avoidThis) const
{
	if (t > 0.0f)
	{
		float teee = 0.f;
		if (m_quadTree[quadIndex].Intersects(rayDirection, rayOrigin, teee))
		{
			int nrOfChildren = m_quadTree[quadIndex].GetNrOfChildren();

			if (nrOfChildren > 0)
			{
				const unsigned int * children = m_quadTree[quadIndex].GetChildren();
				for (int i = 0; i < nrOfChildren; i++)
					_traverseWithRay(rayOrigin, rayDirection, children[i], t, ePtr, avoidThis);
			}
			else
			{
				// Trace Objects
				const std::vector<Drawable*> & objects = m_quadTree[quadIndex].GetDrawables();

				size_t size = objects.size();
				float tTemp = FLT_MAX;

				for (int i = 0; i < size; i++)
				{
					if (_insideRay(rayOrigin, rayDirection, objects[i], tTemp) && tTemp < t)
					{
						if (avoidThis && ePtr)
						{
							if (avoidThis != ePtr)
							{
								ePtr = objects[i];
								t = tTemp;
							}
						}
						else
						{
							ePtr = objects[i];
							t = tTemp;
						}

					}
				}
			}
		}
	}
}

bool QuadTree::_lineWithLineIntersection(const DirectX::XMFLOAT2 & lineOrigin1, const DirectX::XMFLOAT2 & lineEnd1, const DirectX::XMFLOAT2 & lineOrigin2, const DirectX::XMFLOAT2 & lineEnd2, float & t) const
{
	static const float EPSILON = 0.0001f;

	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&lineOrigin1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&lineEnd1);
	
	DirectX::XMVECTOR b = DirectX::XMVectorSubtract(v1, v2);
	v1 = DirectX::XMLoadFloat2(&lineOrigin2);
	v2 = DirectX::XMLoadFloat2(&lineEnd2);

	DirectX::XMVECTOR d = DirectX::XMVectorSubtract(v1, v2);

	float bDotDPerp = DirectX::XMVectorGetX(DirectX::XMVector2Dot(b, d));
	
	if (fabs(bDotDPerp) < EPSILON)
		return false;

	v2 = DirectX::XMLoadFloat2(&lineOrigin1);
	
	DirectX::XMFLOAT2 bDotDPerpFloat;
	bDotDPerpFloat.x = bDotDPerp;
	bDotDPerpFloat.y = bDotDPerp;

	DirectX::XMVECTOR c = DirectX::XMVectorSubtract(v1, v2);
	DirectX::XMVECTOR bDotDPerpVector = DirectX::XMLoadFloat2(&bDotDPerpFloat);
	float tTemp = DirectX::XMVectorGetX(DirectX::XMVectorDivide(DirectX::XMVector2Dot(c, d), bDotDPerpVector));
	
	if (tTemp < 0.0f || tTemp > 1.0f)
		return false;

	float u = DirectX::XMVectorGetX(DirectX::XMVectorDivide(DirectX::XMVector2Dot(c, b), bDotDPerpVector));

	if (u < 0.0f || u > 1.0f)
		return false;

	t = tTemp;

	return true;
}

bool QuadTree::_insideRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, const Quadrant & quadrant) const
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&rayStart);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&rayEnd);
	DirectX::XMFLOAT2 length;
	DirectX::XMStoreFloat2(&length, DirectX::XMVectorSubtract(v2, v1));

	if (_insideAABB(rayStart, length, quadrant))
		return true;

	return false;
}

bool QuadTree::_insideRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, const Drawable * e, float & t) const
{
	DirectX::XMFLOAT2 points[4];
	DirectX::XMFLOAT2 eSize = e->GetSize();
	DirectX::XMFLOAT3 ePos = e->GetPosition();
	points[0] = DirectX::XMFLOAT2(ePos.x, ePos.z);
	points[1] = DirectX::XMFLOAT2(ePos.x + eSize.x, ePos.z);
	points[2] = DirectX::XMFLOAT2(ePos.x + eSize.x, ePos.z + eSize.y);
	points[3] = DirectX::XMFLOAT2(ePos.x, ePos.z + eSize.y);

	if (rayStart.x > points[0].x && rayStart.x < points[2].x
		&&
		rayStart.y > points[0].y && rayStart.y < points[2].y)
	{
		t = 0.0f;
		return true;
	}

	float tTemp = 0.0f;
	bool returnVal = false;
	for (int i = 0; i < 4; i++)
	{
		DirectX::XMFLOAT2 edgeStart = points[i];
		DirectX::XMFLOAT2 edgeEnd = points[(i + 1) % 4];

		if (_lineWithLineIntersection(rayStart, rayEnd, edgeStart, edgeEnd, tTemp) && tTemp < t)
		{
			t = tTemp;
			returnVal = true;
		}
	}

	return returnVal;
}

bool QuadTree::_insideAABB(const DirectX::XMFLOAT2 & min, const DirectX::XMFLOAT2 & size, const Quadrant & quadrant) const
{
	DirectX::BoundingBox bb;
	DirectX::XMVECTOR max = DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&min), DirectX::XMLoadFloat2(&size));
	bb.CreateFromPoints(bb, DirectX::XMLoadFloat2(&min), max);

	return quadrant.Intersects(bb) || quadrant.Intersects(min);
}

bool QuadTree::_insideAABB(const DirectX::XMFLOAT2 & min, const Quadrant & quadrant) const
{
	return quadrant.Intersects(min);
}

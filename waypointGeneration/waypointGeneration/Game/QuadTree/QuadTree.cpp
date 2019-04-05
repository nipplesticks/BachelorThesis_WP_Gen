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

Drawable * QuadTree::LineIntersection(const DirectX::XMFLOAT2 & origin, const DirectX::XMFLOAT2 & direction, float & t, Drawable * avoidThis)
{
	return nullptr;
}

Triangle * QuadTree::PointIntersection(const DirectX::XMFLOAT2 & point, Drawable * avoidThis)
{
	return nullptr;
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
	// TODO Waypoints
}

void QuadTree::_traverseAndPlace(Triangle * e, int quadIndex)
{
	// Todo Triangle
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
				float tTemp = FLT_MAX;

				for (int i = 0; i < size; i++)
				{
					DirectX::BoundingBox bb;
					//bb.CreateFromPoints(&bb, objects[i])
					/*sf::FloatRect fr;
					fr.left = objects[i]->GetPosition().x;
					fr.top = objects[i]->GetPosition().y;
					fr.width = objects[i]->GetSize().x;
					fr.height = objects[i]->GetSize().y;*/

					/*if (fr.contains(point))
					{
						ePtr = objects[i];
					}*/
				}
			}
		}
	}
}

void QuadTree::_traverseWithRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, int quadIndex, float & t, Drawable *& ePtr) const
{
}

bool QuadTree::_lineWithLineIntersection(const DirectX::XMFLOAT2 & lineOrigin1, const DirectX::XMFLOAT2 & lineEnd1, const DirectX::XMFLOAT2 & lineOrigin2, const DirectX::XMFLOAT2 & lineEnd2, float & t) const
{
	return false;
}

bool QuadTree::_insideRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, const Quadrant & quadrant) const
{
	return false;
}

bool QuadTree::_insideRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, const Drawable * e, float & t) const
{
	return false;
}

bool QuadTree::_insideAABB(const DirectX::XMFLOAT2 & min, const DirectX::XMFLOAT2 & size, const Quadrant & quadrant) const
{
	return false;
}

bool QuadTree::_insideAABB(const DirectX::XMFLOAT2 & min, const Quadrant & quadrant) const
{
	return false;
}

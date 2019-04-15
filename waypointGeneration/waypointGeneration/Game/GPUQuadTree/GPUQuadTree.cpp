#include "waypointGenerationPCH.h"
#include "GPUQuadTree.h"


GPUQuadTree::GPUQuadTree()
{
}


GPUQuadTree::~GPUQuadTree()
{
}

DirectX::XMFLOAT2 AddFloat2(const DirectX::XMFLOAT2 & a, const DirectX::XMFLOAT2 & b)
{
	DirectX::XMFLOAT2 c = a;

	c.x += b.x;
	c.y += b.y;

	return c;
}

DirectX::XMFLOAT2 SubtractFloat2(const DirectX::XMFLOAT2 & a, const DirectX::XMFLOAT2 & b)
{
	DirectX::XMFLOAT2 c = a;

	c.x -= b.x;
	c.y -= b.y;

	return c;
}

void GPUQuadTree::BuildTree(float startX, float startY, UINT treeLevel, float worldSize)
{
	using namespace DirectX;
	m_totalTreeByteSize = 0;
	m_leafCounter = 0;
	m_leafIndices.clear();
	m_quadTree.clear();
	m_level = treeLevel;

	size_t numberOfQuads = 0;
	size_t numberOfLeaves = (size_t)std::pow(4, treeLevel);

	for (UINT level = 0; level <= treeLevel; level++)
		numberOfQuads += (size_t)std::pow(4, level);

	XMFLOAT2 startPos(startX, startY);
	XMFLOAT2 size(worldSize, worldSize);

	m_quadTree = std::vector<GPUQuadrant>(numberOfQuads);
	m_leafIndices = std::vector<UINT>(numberOfLeaves);

	UINT index = 0;
	_CreateQuadrant(startPos, AddFloat2(startPos, size), 0, treeLevel == 0, index++);
	
	UINT sizeOfNonLeaf = m_quadTree[0].ByteSize;

	for (UINT level = 1; level <= treeLevel; level++)
	{
		bool isLeaf = level == treeLevel;
		UINT XY = (UINT)std::pow(2, level);
		XMStoreFloat2(&size, XMVectorScale(XMLoadFloat2(&size), 0.5f));

		for (UINT y = 0; y < XY; y++)
		{
			XMFLOAT2 currentPos;
			currentPos.y = startPos.y + size.y * y;
			for (UINT x = 0; x < XY; x++)
			{
				currentPos.x = startPos.x + size.x * x;
				_CreateQuadrant(currentPos, AddFloat2(currentPos, size), level, isLeaf, index);
				UINT parentIndex = _GetQuadrantIndexByWorldPos(currentPos, level - 1);
				m_quadTree[index].ByteStart = m_quadTree[index].ByteSize * index;
				m_quadTree[parentIndex].ChildrenIndices[m_quadTree[parentIndex].NrOfChildren] = index;
				m_quadTree[parentIndex].ChildrenByteAddress[m_quadTree[parentIndex].NrOfChildren++] = sizeOfNonLeaf * index++;
			}
		}

	}


}

void GPUQuadTree::PlaceObjects(const std::vector<Triangle>& triangles)
{
	_clearLeaves();

	m_objectsInLeaves = triangles.size();

	for (UINT i = 0; i < m_objectsInLeaves; i++)
		_traverseAndPlace(triangles[i], i, 0);

	size_t leafSize = m_leafCounter;
	UINT offset = m_quadTree[m_leafIndices[0]].ByteStart;

	for (size_t i = 0; i < leafSize; i++)
	{
		size_t index = m_leafIndices[i];
		m_quadTree[index].CalcSize();
		m_quadTree[index].ByteStart = offset;

		UINT parentIndex = _GetQuadrantIndexByWorldPos(m_quadTree[index].Min, m_quadTree[index].Level - 1);

		if (m_quadTree[parentIndex].NrOfChildren == 4) m_quadTree[parentIndex].NrOfChildren = 0;

		m_quadTree[parentIndex].ChildrenByteAddress[m_quadTree[parentIndex].NrOfChildren++] = offset;

		offset += m_quadTree[index].ByteSize;
	}
	m_totalTreeByteSize = m_quadTree.back().ByteStart + m_quadTree.back().ByteSize;
}

const std::vector<GPUQuadrant>& GPUQuadTree::GetQuadTree() const
{
	return m_quadTree;
}

UINT GPUQuadTree::ByteSize() const
{
	return m_totalTreeByteSize;
}

bool GPUQuadTree::_inside(const DirectX::BoundingBox& bb, const Triangle& tri)
{
	using namespace DirectX;

	
	XMVECTOR v[3];

	for (int i = 0;i < 3; i++)
	{
		XMFLOAT2 _v;

		_v.x = tri.points[i].x;
		_v.y = tri.points[i].z;

		v[i] = XMLoadFloat2(&_v);
	}

	return bb.Intersects(v[0], v[1], v[2]);
}

void GPUQuadTree::_CreateQuadrant(const DirectX::XMFLOAT2& _min, const DirectX::XMFLOAT2& _max, UINT level, bool isLeaf,
	UINT index)
{
	GPUQuadrant gq;
	gq.Min = _min;
	gq.Max = _max;
	gq.Level = level;
	gq.CalcSize();
	m_quadTree[index] = gq;

	if (isLeaf)
		m_leafIndices[m_leafCounter++] = index;

}

UINT GPUQuadTree::_GetQuadrantIndexByWorldPos(const DirectX::XMFLOAT2& worldPos, UINT level)
{
	using namespace DirectX;
	size_t levelStartIndex = 0;
	//size_t levelEndIndex = 0;

	for (UINT i = 0; i < level; i++)
		levelStartIndex += (UINT)std::pow(4, i);
	//levelEndIndex = levelStartIndex + (UINT)std::pow(4, level);

	XMFLOAT2 levelSize;
	XMStoreFloat2(&levelSize, XMVectorSubtract(XMLoadFloat2(&m_quadTree[levelStartIndex].Max), XMLoadFloat2(&m_quadTree[levelStartIndex].Min)));

	levelSize.x = 1.0f / levelSize.x;
	levelSize.y = 1.0f / levelSize.y;
	
	XMFLOAT2 masterNodePos = m_quadTree[0].Min;

	XMVECTOR worldStart = XMLoadFloat2(&masterNodePos);
	XMVECTOR searchPos = XMVectorSubtract(XMLoadFloat2(&worldPos), worldStart);
	XMFLOAT2 index;
	XMStoreFloat2(&index, XMVectorMultiply(searchPos, XMLoadFloat2(&levelSize)));

	UINT XY = (UINT)std::pow(2, level);

	return (UINT)levelStartIndex + (UINT)index.x + (UINT)index.y * XY;
}

void GPUQuadTree::_clearLeaves()
{
	for (UINT i = 0; i < m_leafCounter; i++)
	{
		UINT index = m_leafIndices[i];
		m_quadTree[index].TriangleIndices.clear();
		m_quadTree[index].NrOfObjects = 0;
	}
}

void GPUQuadTree::_traverseAndPlace(const Triangle& tri, UINT triIndex, UINT quadrantIndex)
{
	DirectX::BoundingBox bb;
	bb.CreateFromPoints(bb, 
		DirectX::XMLoadFloat2(&m_quadTree[quadrantIndex].Min),
		DirectX::XMLoadFloat2(&m_quadTree[quadrantIndex].Max));

	if (_inside(bb, tri))
	{
		UINT nrOfChildren = 0;
		if ((nrOfChildren = m_quadTree[quadrantIndex].NrOfChildren) > 0)
		{
			for (UINT i = 0; i < nrOfChildren; i++)
				_traverseAndPlace(tri, triIndex, m_quadTree[quadrantIndex].ChildrenIndices[i]);
		}
		else
		{
			m_quadTree[quadrantIndex].TriangleIndices.push_back(triIndex);
			m_quadTree[quadrantIndex].NrOfObjects++;
		}
	}
}

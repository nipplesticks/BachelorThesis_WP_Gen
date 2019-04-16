#pragma once
#include "../../Rendering/structs.h"

#include <DirectXCollision.h>

struct GPUQuadrant
{
	GPUQuadrant() {};
	GPUQuadrant(const GPUQuadrant & other)
	{
		Copy(other);
	}

	void CalcSize()
	{
		ByteSize = sizeof(*this);
		ByteSize -= sizeof(TriangleIndices) + sizeof(UINT);
		ByteSize += (UINT)TriangleIndices.size() * sizeof(UINT);
	}

	GPUQuadrant & operator=(const GPUQuadrant & other)
	{
		if (this != &other)
			Copy(other);

		return *this;
	}

	void Copy(const GPUQuadrant & other)
	{
		Min = other.Min;
		Max = other.Max;
		Level = other.Level;
		NrOfChildren = other.NrOfChildren;
		ByteSize = other.ByteSize;
		ByteStart = other.ByteStart;
		TriangleIndices = other.TriangleIndices;
		NrOfObjects = other.NrOfObjects;

		for (UINT i = 0; i < NrOfChildren; i++)
		{
			ChildrenIndices[i] = other.ChildrenIndices[i];
			ChildrenByteAddress[i] = other.ChildrenByteAddress[i];
		}
	}

	std::string ToString() const
	{
		std::string str = "";
		str += "ByteSize: " + std::to_string(ByteSize) + "\n";
		str += "ByteStart: " + std::to_string(ByteStart) + "\n";
		str += "Min: " + std::to_string(Min.x) + ", " + std::to_string(Min.y) + "\n";
		str += "Max: " + std::to_string(Max.x) + ", " + std::to_string(Max.y) + "\n";
		str += "Level: " + std::to_string(Level) + "\n";
		str += "NrOfChildren: " + std::to_string(NrOfChildren) + "\n";
		str += "ChildrenIndices: ";
		for (UINT i = 0; i < NrOfChildren; i++)
			str += std::to_string(ChildrenIndices[i]) + ", ";
		str += "\n";
		str += "ChildrenByteAddress: ";
		for (UINT i = 0; i < NrOfChildren; i++)
			str += std::to_string(ChildrenByteAddress[i]) + ", ";
		str += "\n";
		str += "NrOfObjects: " + std::to_string(NrOfObjects) + "\n";
		str += "TriangleIndices: ";
		for (UINT i = 0; i < NrOfObjects; i++)
			str += std::to_string(TriangleIndices[i]) + ", ";
		str += "\n";
		return str;
	}

	UINT ByteSize = 0;
	UINT ByteStart = 0;

	DirectX::XMFLOAT2 Min = DirectX::XMFLOAT2(0.0f, 0.0f);
	DirectX::XMFLOAT2 Max = DirectX::XMFLOAT2(0.5f, 0.5f);

	UINT Level = 0;
	UINT NrOfChildren = 0;
	UINT ChildrenIndices[4] = { 0, 0, 0, 0 };
	UINT ChildrenByteAddress[4] = { 0, 0, 0, 0 };
	UINT NrOfObjects = 0;
	std::vector<UINT> TriangleIndices;
};

class GPUQuadTree
{
public:
	GPUQuadTree();
	~GPUQuadTree();

	void BuildTree(float startX = 0.0f, float startY = 0.0f, UINT treeLevel = 7, float worldSize = 1024.0f); 
	void PlaceObjects(const std::vector<Triangle> & triangles);
	const std::vector<GPUQuadrant> & GetQuadTree() const;
	UINT ByteSize() const;

	std::string ToString() const;


private:
	std::vector<GPUQuadrant>	m_quadTree;
	std::vector<UINT>			m_leafIndices;

	UINT m_leafCounter = 0;
	UINT m_objectsInLeaves = 0;
	UINT m_level = 0;
	UINT m_totalTreeByteSize = 0;

private:
	bool _inside(const DirectX::BoundingBox & bb, const Triangle & tri);
	void _CreateQuadrant(const DirectX::XMFLOAT2 & _min, const DirectX::XMFLOAT2 & _max, UINT level, bool isLeaf, UINT index);
	UINT _GetQuadrantIndexByWorldPos(const DirectX::XMFLOAT2 & worldPos, UINT level);

	void _clearLeaves();

	void _traverseAndPlace(const Triangle & tri, UINT triIndex, UINT quadrantIndex);
};




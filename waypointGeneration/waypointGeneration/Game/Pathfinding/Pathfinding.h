#pragma once
#include "..\QuadTree\QuadTree.h"

class Pathfinding
{
public:
	Pathfinding();
	~Pathfinding();

	std::vector<DirectX::XMFLOAT2> FindPath(
		const DirectX::XMFLOAT3 & source,
		const DirectX::XMFLOAT3 & destination,
		QuadTree & blockedTriangles) const;

private:
	struct Node
	{
		Node()
		{
			fCost = FLT_MAX;
			gCost = FLT_MAX;
			hCost = FLT_MAX;
		}

		Node(float _gCost, float _hCost)
		{
			fCost = _gCost + _hCost;
			gCost = _gCost;
			hCost = _hCost;
		}
		
		bool operator<(const Node & other) const
		{
			return fCost < other.fCost;
		}

		float fCost;
		float gCost;
		float hCost;
	};

private:
	std::vector<DirectX::XMFLOAT2> _findPath(
		const DirectX::XMFLOAT2 & source,
		const DirectX::XMFLOAT2 & destination,
		QuadTree & blockedTriangles) const;
};

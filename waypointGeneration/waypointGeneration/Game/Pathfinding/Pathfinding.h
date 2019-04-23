#pragma once
#include "..\QuadTree\QuadTree.h"

class Pathfinding
{
public:
	/*Pathfinding();
	~Pathfinding();*/

	static std::vector<DirectX::XMFLOAT2> FindPath(
		const DirectX::XMFLOAT3 & source,
		const DirectX::XMFLOAT3 & destination,
		QuadTree & blockedTriangles);

private:
	struct Node
	{
		Node()
		{
			parent = nullptr;
			target = nullptr;
			fCost = FLT_MAX;
			gCost = FLT_MAX;
			hCost = FLT_MAX;
		}

		Node(Waypoint * _parentPointer, Waypoint * _target, float _gCost, float _hCost)
		{
			parent = _parentPointer;
			target = _target;
			fCost = _gCost + _hCost;
			gCost = _gCost;
			hCost = _hCost;
		}
		
		bool operator<(const Node & other) const
		{
			return fCost < other.fCost;
		}

		Node & operator=(const Node & other)
		{
			if (this != &other)
			{
				parent = other.parent;
				target = other.target;
				fCost = other.fCost;
				gCost = other.gCost;
				hCost = other.hCost;
			}
			return *this;
		}

		bool operator==(const Node & other)
		{
			return target == other.target;
		}

		Waypoint * parent = nullptr;
		Waypoint * target = nullptr;

		float fCost;
		float gCost;
		float hCost;
	};

private:
	static std::vector<DirectX::XMFLOAT2> _findPath(
		const DirectX::XMFLOAT2 & source,
		const DirectX::XMFLOAT2 & destination,
		QuadTree & blockedTriangles);


	static float _calcHCost(const DirectX::XMFLOAT2 & a, const DirectX::XMFLOAT2 & b);

};

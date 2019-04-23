#include "..\..\waypointGenerationPCH.h"
#include "Pathfinding.h"

Pathfinding::Pathfinding()
{
}

Pathfinding::~Pathfinding()
{
}

std::vector<DirectX::XMFLOAT2> Pathfinding::FindPath(
	const DirectX::XMFLOAT3& source,
	const DirectX::XMFLOAT3& destination,
	QuadTree& blockedTriangles) const
{
	DirectX::XMFLOAT2 src = DirectX::XMFLOAT2(source.x, source.y);
	DirectX::XMFLOAT2 dest = DirectX::XMFLOAT2(destination.x, destination.y);
	std::vector<DirectX::XMFLOAT2> path = _findPath(src, dest, blockedTriangles);

	return path;
}

std::vector<DirectX::XMFLOAT2> Pathfinding::_findPath(
	const DirectX::XMFLOAT2& source,
	const DirectX::XMFLOAT2& destination,
	QuadTree& blockedTriangles) const
{
	std::vector<DirectX::XMFLOAT2> path;
	DirectX::XMFLOAT2 dummy;

	DirectX::XMFLOAT2 current = source;
	
	while (true)
	{
		// Check if destination is in sight from the current location
		Triangle * tri = blockedTriangles.LineIntersectionTriangle(current, destination, true, dummy);
		if (tri == nullptr)
		{
			path.push_back(destination);
			return path;
		}
		else
		{
			// Traverse waypoints to find destination
		}
	}

	return std::vector<DirectX::XMFLOAT2>();
}

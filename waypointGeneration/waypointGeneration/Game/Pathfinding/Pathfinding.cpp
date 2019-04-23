#include "waypointGenerationPCH.h"
#include "Pathfinding.h"
#include <algorithm>

std::vector<DirectX::XMFLOAT2> Pathfinding::FindPath(
	const DirectX::XMFLOAT3& source,
	const DirectX::XMFLOAT3& destination,
	QuadTree& blockedTriangles)
{
	DirectX::XMFLOAT2 src = DirectX::XMFLOAT2(source.x, source.z);
	DirectX::XMFLOAT2 dest = DirectX::XMFLOAT2(destination.x, destination.z);
	std::vector<DirectX::XMFLOAT2> path = _findPath(src, dest, blockedTriangles);

	return path;
}

std::vector<DirectX::XMFLOAT2> Pathfinding::_findPath(
	const DirectX::XMFLOAT2& source,
	const DirectX::XMFLOAT2& destination,
	QuadTree& blockedTriangles)
{
	
	std::vector<DirectX::XMFLOAT2> path;
	DirectX::XMFLOAT2 dummy;

	Triangle * tri = blockedTriangles.LineIntersectionTriangle(source, destination, true, dummy);
	if (tri == nullptr)
	{
		path.push_back(destination);
		return path;
	}

	std::unordered_map<Waypoint*, Node> visited;
	std::map<Node, Waypoint*> openList;

	Waypoint * startWaypoint = nullptr;
	float rad = 1;
	while (startWaypoint == nullptr)
	{
		startWaypoint = blockedTriangles.FindClosestWaypoint(
			DirectX::XMFLOAT3(source.x, 0.0f, source.y),
			rad *= 2);

		if (rad > 32)
			return std::vector<DirectX::XMFLOAT2>();
	}

	Node current(nullptr, startWaypoint, 0.0f, _calcHCost(startWaypoint->GetPosition(), destination));
	//path.push_back(current.target->GetPosition());

	visited.insert(std::make_pair(current.target, current));
	openList.insert(std::make_pair(current, current.target));

	while (!openList.empty())
	{
		current = openList.begin()->first;
		openList.erase(current);

		// Check if destination is in sight from the current location
		tri = blockedTriangles.LineIntersectionTriangle(current.target->GetPosition(), destination, true, dummy);
		if (tri == nullptr)
		{
			Waypoint * p = current.target;
			while (p)
			{
				DirectX::XMFLOAT2 pos = visited[p].target->GetPosition();
				path.push_back(pos);
				p = visited[p].parent;
			}
			std::reverse(path.begin(), path.end());
			path.push_back(destination);
			return path;
		}

		// Traverse waypoints to find destination
		std::map<int, Waypoint::WaypointConnection> * currentConnections = current.target->GetConnections();

		for (auto & c : *currentConnections)
		{
			if (visited.find(c.second.wp) == visited.end())
			{
				Node n(current.target,
					c.second.wp,
					current.gCost + c.second.connectionCost,
					_calcHCost(current.target->GetPosition(), c.second.wp->GetPosition()));
				visited.insert(std::make_pair(c.second.wp, n));
				openList.insert(std::make_pair(n, c.second.wp));
			}
		}
	}

	return std::vector<DirectX::XMFLOAT2>();
}

float Pathfinding::_calcHCost(const DirectX::XMFLOAT2 & a, const DirectX::XMFLOAT2 & b)
{
	return DirectX::XMVectorGetX(
		DirectX::XMVector2LengthSq(
			DirectX::XMVectorSubtract(
				DirectX::XMLoadFloat2(&a),
				DirectX::XMLoadFloat2(&b)
			)));
}

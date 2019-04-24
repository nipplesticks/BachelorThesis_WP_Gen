#include "waypointGenerationPCH.h"
#include "Pathfinding.h"
#include <algorithm>


Pathfinding::Pathfinding()
{
}

Pathfinding::~Pathfinding()
{
	m_isRunning = false;
	for (auto & t : m_pool)
	{
		if (t.second->worker.joinable())
			delete t.second;
	}
	m_pool.clear();
}

void Pathfinding::Terminate()
{
	m_isRunning = false;
	m_condition.notify_all();
	for (auto & t : m_pool)
	{
		delete t.second;
	}
	m_pool.clear();
}

void Pathfinding::InitiateThreadpool(int count)
{
	m_threadCount = count;
	m_isRunning = true;
	m_workId = 0;

	for (int i = 0; i < count; i++)
	{		
		std::function<void()> func = std::bind(&Pathfinding::WaitingForWork, this);
		WorkerThread * wt = new WorkerThread(func);
		m_pool.insert(std::make_pair(wt->worker.get_id(), wt));
	}
}

UINT64 Pathfinding::RequestPath(const DirectX::XMFLOAT3& source, const DirectX::XMFLOAT3& destination,
	QuadTree& blockedTriangles)
{
	ThreadArguments tA;
	tA.source = source;
	tA.destination = destination;
	tA.blockedTriangles = &blockedTriangles;
	UINT64 id;
	{
		std::unique_lock<std::mutex> lock(m_workMutex);
		m_work.push(std::make_pair(tA, id = m_workId++));
	}
	m_condition.notify_one();

	return id;
}

std::vector<DirectX::XMFLOAT2> Pathfinding::GetPath(UINT64 pathId)
{
	std::unique_lock<std::mutex> lock(m_workIdMutex);

	/*while (!lock.owns_lock())
	{
		lock.lock();
	}*/

	std::vector<DirectX::XMFLOAT2> results = m_results[pathId];
	m_results.erase(pathId);

	//lock.unlock();

	return results;
}

bool Pathfinding::IsPathDone(UINT64 pathId)
{
	std::unique_lock<std::mutex> lock(m_workIdMutex);

	/*while (!lock.owns_lock())
	{
		lock.lock();
	}
	*/
	auto it = m_results.find(pathId);
	bool isDone = it != m_results.end();

	//lock.unlock();

	return isDone;
}

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

std::vector<DirectX::XMFLOAT3> Pathfinding::FindPath2(const DirectX::XMFLOAT3& source,
	const DirectX::XMFLOAT3& destination, QuadTree& blockedTriangles)
{
	DirectX::XMFLOAT2 src = DirectX::XMFLOAT2(source.x, source.z);
	DirectX::XMFLOAT2 dest = DirectX::XMFLOAT2(destination.x, destination.z);
	std::vector<DirectX::XMFLOAT3> path = _findPath2(src, dest, blockedTriangles);

	return path;
}

std::vector<DirectX::XMFLOAT2> Pathfinding::_findPath(
	const DirectX::XMFLOAT2& source,
	DirectX::XMFLOAT2& destination,
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
	Waypoint * endWaypoint = nullptr;
	float rad = 1;
	while (startWaypoint == nullptr)
	{
		startWaypoint = blockedTriangles.FindClosestWaypoint(
			DirectX::XMFLOAT3(source.x, 0.0f, source.y),
			rad *= 2);

		if (rad > 32)
			return std::vector<DirectX::XMFLOAT2>();
	}

	if (blockedTriangles.PointInsideTriangle(destination, true))
	{
		rad = 1;
		while (endWaypoint == nullptr)
		{
			endWaypoint = blockedTriangles.FindClosestWaypoint(
				DirectX::XMFLOAT3(destination.x, 0.0f, destination.y),
				rad *= 2, false);

			if (rad > 32)
				return std::vector<DirectX::XMFLOAT2>();
		}

		destination = endWaypoint->GetPosition();

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

			bool done = false;
			int index = 0;
			while (index < path.size() - 1)
			{
				tri = blockedTriangles.LineIntersectionTriangle(source, path[index], true, dummy);
				if (tri)
					break;
				index++;
			}
			index--;
			if (index >= 0)
				path.erase(path.begin(), path.begin() + index);

			return path;
		}

		// Traverse waypoints to find destination
		std::map<int, Waypoint::WaypointConnection> * currentConnections = current.target->GetConnections();

		for (auto & c : *currentConnections)
		{
			if (visited.find(c.second.wp) == visited.end()) // Dont exist in map
			{
				Node n(current.target,
					c.second.wp,
					current.gCost + c.second.connectionCost,
					_calcHCost(c.second.wp->GetPosition(), destination));
				visited.insert(std::make_pair(c.second.wp, n));
				openList.insert(std::make_pair(n, c.second.wp));
			}
		}
	}

	return std::vector<DirectX::XMFLOAT2>();
}

std::vector<DirectX::XMFLOAT3> Pathfinding::_findPath2(const DirectX::XMFLOAT2& source,
	DirectX::XMFLOAT2& destination, QuadTree& blockedTriangles)
{
	std::vector<DirectX::XMFLOAT3> path;
	DirectX::XMFLOAT2 dummy;

	Triangle * tri = blockedTriangles.LineIntersectionTriangle(source, destination, true, dummy);
	if (tri == nullptr)
	{
		path.push_back(DirectX::XMFLOAT3(destination.x, 0.0f, destination.y));
		return path;
	}

	std::unordered_map<Waypoint*, Node> visited;
	std::map<Node, Waypoint*> openList;

	Waypoint * startWaypoint = nullptr;
	Waypoint * endWaypoint = nullptr;
	float rad = 1;
	while (startWaypoint == nullptr)
	{
		startWaypoint = blockedTriangles.FindClosestWaypoint(
			DirectX::XMFLOAT3(source.x, 0.0f, source.y),
			rad *= 2);

		if (rad > 32)
			return std::vector<DirectX::XMFLOAT3>();
	}

	if (blockedTriangles.PointInsideTriangle(destination, true))
	{
		rad = 1;
		while (endWaypoint == nullptr)
		{
			endWaypoint = blockedTriangles.FindClosestWaypoint(
				DirectX::XMFLOAT3(destination.x, 0.0f, destination.y),
				rad *= 2, false);

			if (rad > 32)
				return std::vector<DirectX::XMFLOAT3>();
		}

		destination = endWaypoint->GetPosition();
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
				
				path.push_back(DirectX::XMFLOAT3(pos.x, visited[p].target->GetHeightVal(), pos.y));

				p = visited[p].parent;
			}
			std::reverse(path.begin(), path.end());
			path.push_back(DirectX::XMFLOAT3(destination.x, 0.0f, destination.y));

			bool done = false;
			int index = 0;
			while (index < path.size() - 1)
			{
				tri = blockedTriangles.LineIntersectionTriangle(source, DirectX::XMFLOAT2(path[index].x, path[index].z), true, dummy);
				if (tri)
					break;
				index++;
			}
			index--;
			if (index >= 0)
				path.erase(path.begin(), path.begin() + index);

			return path;
		}

		// Traverse waypoints to find destination
		std::map<int, Waypoint::WaypointConnection> * currentConnections = current.target->GetConnections();

		for (auto & c : *currentConnections)
		{
			if (visited.find(c.second.wp) == visited.end()) // Dont exist in map
			{
				Node n(current.target,
					c.second.wp,
					current.gCost + c.second.connectionCost,
					_calcHCost(c.second.wp->GetPosition(), destination));
				visited.insert(std::make_pair(c.second.wp, n));
				openList.insert(std::make_pair(n, c.second.wp));
			}
		}
	}

	return std::vector<DirectX::XMFLOAT3>();
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

void Pathfinding::WaitingForWork()
{
	std::function<std::vector<DirectX::XMFLOAT2>(const DirectX::XMFLOAT3 &, const DirectX::XMFLOAT3 &, QuadTree &)> work(&Pathfinding::FindPath);
	UINT64 workId;
	ThreadArguments tA;

	while (m_isRunning)
	{
		{
			std::unique_lock<std::mutex> lock(m_workMutex);
			m_condition.wait(lock, [this] { return !m_work.empty() || !m_isRunning; });
			if (!m_isRunning)
				return;
			m_pool[std::this_thread::get_id()]->isDone = false;
			tA = m_work.front().first;
			workId = m_work.front().second;
			m_work.pop();
		}

		std::vector<DirectX::XMFLOAT2> returnValue = work(tA.source, tA.destination, *tA.blockedTriangles);
		m_pool[std::this_thread::get_id()]->isDone = true;

		std::unique_lock<std::mutex> lock(m_workIdMutex);
		
		/*while (!lock.owns_lock())
		{
			lock.lock();
		}*/

		m_results.insert(std::make_pair(workId, returnValue));

		//lock.unlock();
	}
}

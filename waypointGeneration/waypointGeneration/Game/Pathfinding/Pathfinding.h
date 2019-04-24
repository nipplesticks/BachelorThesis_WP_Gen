#pragma once
#include "..\QuadTree\QuadTree.h"
#include <queue>
#include <mutex>

class Pathfinding
{
public:
	Pathfinding();
	~Pathfinding();

	void Terminate();

	void InitiateThreadpool(int count);

	UINT64 RequestPath(
		const DirectX::XMFLOAT3 & source,
		const DirectX::XMFLOAT3 & destination,
		QuadTree & blockedTriangles);

	std::vector<DirectX::XMFLOAT2> GetPath(UINT64 pathId);

	bool IsPathDone(UINT64 pathId);

	static std::vector<DirectX::XMFLOAT2> FindPath(
		const DirectX::XMFLOAT3 & source,
		const DirectX::XMFLOAT3 & destination,
		QuadTree & blockedTriangles);

	static std::vector<DirectX::XMFLOAT3> FindPath2(
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

//Thread pool
private:
	struct WorkerThread
	{
		std::thread worker;
		UINT64 workId;
		bool isDone;

		WorkerThread(std::function<void()> func)
		{
			worker = std::thread(func);
			workId = 0;
			isDone = false;
		}
		~WorkerThread()
		{
			worker.join();
		}
	};

	struct ThreadArguments
	{
		DirectX::XMFLOAT3 source;
		DirectX::XMFLOAT3 destination;
		QuadTree * blockedTriangles;
	};

	int m_threadCount;
	bool m_isRunning;
	UINT64 m_workId;
	std::map<std::thread::id, WorkerThread*> m_pool;

	std::queue<std::pair<ThreadArguments, UINT64>> m_work;
	std::mutex m_workMutex;
	std::mutex m_workIdMutex;
	std::condition_variable m_condition;
	std::map <UINT64, std::vector<DirectX::XMFLOAT2>> m_results;

private:
	static std::vector<DirectX::XMFLOAT2> _findPath(
		const DirectX::XMFLOAT2 & source,
		DirectX::XMFLOAT2 & destination,
		QuadTree & blockedTriangles);
	static std::vector<DirectX::XMFLOAT3> _findPath2(
		const DirectX::XMFLOAT2 & source,
		DirectX::XMFLOAT2 & destination,
		QuadTree & blockedTriangles);

	static float _calcHCost(const DirectX::XMFLOAT2 & a, const DirectX::XMFLOAT2 & b);

// Thread pool
private:
	void WaitingForWork();
};

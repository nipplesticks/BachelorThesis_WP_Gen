#include "waypointGenerationPCH.h"
#include "ThreadPool.h"

void ThreadPool::WaitingForWork()
{
	while (true)
	{

	}
}

ThreadPool::ThreadPool(int threadCount)
{
	m_threadCount = threadCount;
}

ThreadPool::~ThreadPool()
{
}

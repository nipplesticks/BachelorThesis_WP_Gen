#pragma once
#include <queue>
#include "Task.h"

class ThreadPool
{
private:
	int m_threadCount;

	// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

private:
	std::queue<std::thread> m_pool;
	std::queue<Task> m_work;
	std::queue<Task> m_finishedWork;

	void WaitingForWork();

public:
	ThreadPool(int threadCount = 4);
	~ThreadPool();

	void AddTask(std::function<void()> task);
};
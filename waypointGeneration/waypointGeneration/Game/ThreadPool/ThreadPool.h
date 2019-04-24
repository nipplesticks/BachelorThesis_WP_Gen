#pragma once
#include <queue>
#include <mutex>
#include <map>
#include "..\QuadTree\QuadTree.h"

template <class R, class ... Args>
class ThreadPool
{
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
	};

	int m_threadCount;
	bool m_isRunning;
	static UINT64 m_workId;
	std::map<std::thread::id, WorkerThread*> m_pool;

	std::queue<std::pair<std::function<R(Args...)>, UINT64>> m_work;
	std::mutex m_workMutex;
	std::mutex m_workIdMutex;
	std::condition_variable m_condition;
	std::map <UINT64, R> m_results;

private:
	void WaitingForWork();

public:
	ThreadPool(int threadCount = 4);
	~ThreadPool();

	UINT64 ScheduleTask(std::function<std::function<R(Args...)>> func, Args ... arg);
	R GetWorkResults(UINT64 workId);
	bool IsWorkDone(UINT64 workId);
};

template <class R, class ... Args>
void ThreadPool<R, Args...>::WaitingForWork()
{
	std::function<R(Args...)> work;
	UINT64 workId;

	while (m_isRunning)
	{
		{
			std::unique_lock<std::mutex> lock(m_workMutex);
			m_condition.wait(lock, [this] { return !m_work.empty(); });
			m_pool[std::this_thread::get_id()]->isDone = false;
			work = m_work.front().first;
			workId = m_work.front().second;
			m_work.pop();
		}

		R returnValue = work();
		m_pool[std::this_thread::get_id()]->isDone = true;

		std::unique_lock<std::mutex> lock(m_workIdMutex);

		while (!lock.owns_lock())
		{
			lock.try_lock();
		}

		m_results.insert(std::make_pair(workId, returnValue));

		lock.unlock();
	}
}

template <class R, class ... Args>
ThreadPool<R, Args...>::ThreadPool(int threadCount)
{
	m_threadCount = threadCount;
	m_isRunning = true;

	for (int i = 0; i < m_threadCount; i++)
	{
		std::function<void()> func = std::bind(&ThreadPool::WaitingForWork, this);
		WorkerThread * wt = new WorkerThread(func);
		m_pool.insert(std::make_pair(wt->worker.get_id(), wt));
	}
}

template <class R, class ... Args>
ThreadPool<R, Args...>::~ThreadPool()
{
	m_isRunning = false;
	for (auto & t : m_pool)
	{
		t.second->worker.join();
		delete t.second;
	}
}

template <class R, class... Args>
UINT64 ThreadPool<R, Args...>::ScheduleTask(
	std::function<std::function<R(Args...)>> task, Args ... arg)
{
	UINT64 id;
	{
		std::unique_lock<std::mutex> lock(m_workMutex);
		m_work.push(std::make_pair(std::bind(task, arg), id = m_workId++));
	}
	m_condition.notify_one();

	return id;
}

template <class R, class ... Args>
R ThreadPool<R, Args...>::GetWorkResults(UINT64 workId)
{
	std::unique_lock<std::mutex> lock(m_workIdMutex);

	while (!lock.owns_lock())
	{
		lock.try_lock();
	}

	R results = m_results[workId];
	m_results.erase(workId);

	lock.unlock();

	return results;
}

template <class R, class ... Args>
bool ThreadPool<R, Args...>::IsWorkDone(UINT64 workId)
{
	std::unique_lock<std::mutex> lock(m_workIdMutex);

	while (!lock.owns_lock())
	{
		lock.try_lock();
	}

	bool isDone;
	auto it = m_results.find(workId);

	isDone = it != m_results.end();

	lock.unlock();

	return isDone;
}

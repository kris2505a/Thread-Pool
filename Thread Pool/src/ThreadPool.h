#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

class ThreadPool {
public:
	ThreadPool(int n);
	~ThreadPool();

	void init();
	void shutdown();
	void addJobs(std::function<void()> func);

private:
	void work();
	bool getJobs(std::function<void()>& inFunc);

private:
	std::vector <std::thread> m_workers;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::queue <std::function<void()>> m_jobs;

	std::atomic<bool> m_exit;
	bool m_active;
};
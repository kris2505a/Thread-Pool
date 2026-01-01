#include "ThreadPool.h"
#include <print>
#include <chrono>

using namespace std::chrono_literals;


ThreadPool::ThreadPool(int n) {
	m_active = false;
	m_exit.store(true);
	m_workers = std::vector <std::thread>(n);

	//init();
}

ThreadPool::~ThreadPool() {
	if (m_active)
		shutdown();
}

void ThreadPool::init() {
	m_active = true;
	for(auto& worker : m_workers)
		worker = std::thread(&ThreadPool::work, this);

}



void ThreadPool::shutdown() {
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_exit.store(true);
	}
	m_cv.notify_all();

	for (auto& worker : m_workers) {
		if (worker.joinable())
			worker.join();
	}
	
	std::println("ended!");

	m_active = false;
}

void ThreadPool::addJobs(std::function<void()> func) {
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_jobs.push(func);
	}
	m_cv.notify_one();
}

void ThreadPool::work() {
	while (true) {

		std::function<void()> job;
		if (!getJobs(job))
			return;
		job();
	}
}

bool ThreadPool::getJobs(std::function<void()>& inFunc) {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cv.wait(
		lock,
		[this] {
			return !m_jobs.empty() || m_exit;
		}
	);

	if (!m_jobs.empty()) {
		inFunc = std::move(m_jobs.front());
		m_jobs.pop();
		return true;
	}
	return false;
}



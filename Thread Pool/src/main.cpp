#include <print>
#include "ThreadPool.h"
#include <chrono>

void test_thread_pool(int size) {
    ThreadPool pool(size);   // ctor starts workers

    auto job = [] {
        volatile int x = 0;
        for (int i = 0; i < 100'000; ++i)
            x += i;
        };

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < 100'000; ++i) {
        pool.addJobs(job);
    }

    pool.shutdown();   // waits for all jobs

    auto end = std::chrono::steady_clock::now();

    std::println(
        "Thread pool: {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    );
}



int main() {
    test_thread_pool(3);
    test_thread_pool(5);
    test_thread_pool(10);
}
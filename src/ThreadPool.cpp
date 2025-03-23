#include "ThreadPool.h"

ThreadPool::ThreadPool(const size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    stop = true;
    cv.notify_all();
    for (auto &worker : workers) {
        if (worker.joinable())
            worker.join();
    }
}

size_t ThreadPool::numThreads() const { return workers.size(); }
void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this] { return stop || !taskQueue.empty(); });
            if (stop && taskQueue.empty())
                return;
            task = std::move(taskQueue.front());
            taskQueue.pop();
        }
        task();
    }
}

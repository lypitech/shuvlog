#ifndef SHUVLOG_THREADSAFEQUEUE_H
#define SHUVLOG_THREADSAFEQUEUE_H

#include <cstddef>
#include <mutex>
#include <queue>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue final
{
public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(T value)
    {
        {
            std::lock_guard lock(_mutex);
            _queue.push(std::move(value));
        } // nested scope so that mutex is released without waiting to notify.
        _cvar.notify_one();
    }

    std::optional<T> pop()
    {
        std::lock_guard lock(_mutex);

        if (_queue.empty()) {
            return std::nullopt;
        }

        T value = std::move(_queue.front());

        _queue.pop();
        return value;
    }

    bool empty() const
    {
        std::lock_guard lock(_mutex);
        return _queue.empty();
    }

    std::size_t size() const
    {
        std::lock_guard lock(_mutex);
        return _queue.size();
    }

    void waitForData(
        std::chrono::milliseconds timeout,
        const std::atomic<bool>& runningFlag
    )
    {
        std::unique_lock lock(_mutex);

        _cvar.wait_for(lock, timeout, [&] {
            return !_queue.empty() || !runningFlag.load();
        });
    }

    void drainTo(std::vector<T>& out)
    {
        std::lock_guard lock(_mutex);

        while (!_queue.empty()) {
            out.emplace_back(std::move(_queue.front()));
            _queue.pop();
        }
    }

    void notifyAll()
    {
        _cvar.notify_all();
    }

private:
    mutable std::mutex _mutex;
    std::condition_variable _cvar;
    std::queue<T> _queue;
};

#endif //SHUVLOG_THREADSAFEQUEUE_H

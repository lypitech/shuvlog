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

    /**
     * @brief   Adds an element to the back of the queue.
     *
     * @param   value   The value to push to the queue
     */
    void push(T value)
    {
        {
            std::lock_guard lock(_mutex);
            _queue.push(std::move(value));
        } // nested scope so that mutex is released without waiting to notify.
        _cvar.notify_one();
    }

    /**
     * @brief   Removes the first element of the queue.
     *
     * @return  An @code std::optional<T>@endcode containing
     *          the popped value, or @code std::nullopt@endcode
     *          if the queue is empty.
     */
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

    /**
     * @return  @code true@endcode if no items are stored in the queue.
     */
    bool empty() const
    {
        std::lock_guard lock(_mutex);
        return _queue.empty();
    }

    /**
     * @return  The number of elements in the queue.
     */
    std::size_t size() const
    {
        std::lock_guard lock(_mutex);
        return _queue.size();
    }

    /**
     * @brief   Blocks until data is available or until a timeout occurs.
     *
     * Mainly used by a consumer thread. The caller controls shutdown by
     * providing an external atomic @code runningFlag@endcode; if it
     * becomes false, waiting ends even when the queue remains empty.
     *
     * @param   timeout     The maximum time to wait, in milliseconds
     * @param   runningFlag A flag that indicates whether the system is still active
     */
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

    /**
     * @brief Removes all elements from the queue and appends them to
     * @code out@endcode.
     *
     * This is useful during shutdown when remaining queued data must be flushed.
     *
     * @param   out The vector to which all queued elements will be moved
     */
    void drainTo(std::vector<T>& out)
    {
        std::lock_guard lock(_mutex);

        while (!_queue.empty()) {
            out.emplace_back(std::move(_queue.front()));
            _queue.pop();
        }
    }

    /**
     * @brief   Notifies all threads waiting on the condition variable.
     *
     * Used when shutting down to unblock all waiting data.
     */
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

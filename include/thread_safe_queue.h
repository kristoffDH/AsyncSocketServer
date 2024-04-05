//
// Created by kristoff on 4/3/24.
//

#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

namespace thread_safe_queue {
template <class T>
class Queue {
  public:
    Queue() : queue(), mutex(), cv() {}
    ~Queue() = default;

    void Push(T t) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(t);
        cv.notify_one();
    }

    T Pop() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]() { return !queue.empty(); });
        T val = queue.front();
        queue.pop();
        return val;
    }

    bool IsEmpty() {
        return queue.empty();
    }

  private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cv;
};
}  // namespace thread_safe_queue

#endif  // THREAD_SAFE_QUEUE_H

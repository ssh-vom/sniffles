#pragma once

#include <cstddef>
#include <mutex>
#include <queue>

namespace sniffles::util {
template <typename T>
class ThreadSafeQueue {
public:
  void Push(const T &value) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
  }

  bool TryPop(T &out) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      return false;
    }
    out = queue_.front();
    queue_.pop();
    return true;
  }

  std::size_t Size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

private:
  mutable std::mutex mutex_;
  std::queue<T> queue_;
};
} // namespace sniffles::util

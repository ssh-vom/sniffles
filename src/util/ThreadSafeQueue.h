#pragma once

#include <boost/lockfree/queue.hpp>
#include <cstddef>
#include <memory>
#include <optional>

namespace sniffles::util {

template <typename T>
class ThreadSafeQueue {
public:
  explicit ThreadSafeQueue(std::size_t capacity = 10000)
      : queue_(capacity) {}

  void Push(const T &value) {
    T *pvalue = new T(value);
    if (!queue_.push(pvalue)) {
      delete pvalue;
    }
  }

  void Push(T &&value) {
    T *pvalue = new T(std::move(value));
    if (!queue_.push(pvalue)) {
      delete pvalue;
    }
  }

  bool TryPop(T &out) {
    T *pvalue = nullptr;
    if (queue_.pop(pvalue)) {
      out = std::move(*pvalue);
      delete pvalue;
      return true;
    }
    return false;
  }

  std::optional<T> TryPop() {
    T *pvalue = nullptr;
    if (queue_.pop(pvalue)) {
      T value = std::move(*pvalue);
      delete pvalue;
      return value;
    }
    return std::nullopt;
  }

  bool IsEmpty() const { return queue_.empty(); }

  std::size_t Capacity() const { return queue_.capacity(); }

private:
  boost::lockfree::queue<T *> queue_;
};

} // namespace sniffles::util

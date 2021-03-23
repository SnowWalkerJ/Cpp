//
// Created by 陈秉烜 on 2021/3/22.
//

#ifndef CPP_HEADER_SHAREDPTR_H_
#define CPP_HEADER_SHAREDPTR_H_
#include <type_traits>
#include <stdexcept>

namespace cpp {
template<typename T>
class SharedPtr {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using counter = size_t;

  SharedPtr() noexcept : ptr_(nullptr), count_(nullptr) {}
  SharedPtr(const_pointer ptr) noexcept : ptr_(ptr), count_(new int(1)) {}
  SharedPtr(const SharedPtr &other) {
    copy(other);
  }
  SharedPtr(SharedPtr &&other) {
    move(std::move(other));
  }
  const SharedPtr &operator=(const SharedPtr &) = delete;
  const SharedPtr &operator=(SharedPtr &&other) {
    destroy();
    move(std::move(other));
    return *this;
  }

  pointer operator->() {
    if (has_value()) {
      return ptr_;
    } else {
      throw std::runtime_error("accessing empty SharedPtr");
    }
  }

  const_pointer operator->() const {
    if (has_value()) {
      return ptr_;
    } else {
      throw std::runtime_error("accessing empty SharedPtr");
    }
  }

  reference operator->() {
    if (has_value()) {
      return *ptr_;
    } else {
      throw std::runtime_error("accessing empty SharedPtr");
    }
  }
  const_reference operator->() const {
    if (has_value()) {
      return *ptr_;
    } else {
      throw std::runtime_error("accessing empty SharedPtr");
    }
  }

  template<typename ...Args>
  SharedPtr &Emplace(Args&&...args) {
    destroy();
    construct(std::forward<Args>(args)...);
    return *this;
  }

  explicit operator bool() const { return has_value(); }

  ~SharedPtr() {
    destroy();
  }
 private:
  template<typename...Args>
  void construct(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
    count_ = new counter(1);
    ptr_ = new T(std::forward<Args>(args)...);
  }
  void copy(const SharedPtr &other) {
    if (other.has_value()) {
      ptr_ = other.ptr_;
      count_ = other.count_;
      inc();
    } else {
      ptr_ = nullptr;
      count_ = nullptr;
    }
  }
  void move(SharedPtr &&other) {
    if (other.has_value()) {
      ptr_ = other.ptr_;
      count_ = other.count_;
      other.ptr_ = nullptr;
      other.count_ = nullptr;
    } else {
      ptr_ = nullptr;
      count_ = nullptr;
    }
  }
  counter count() const {
    return count_ != nullptr ? *count_ : 0;
  }
  bool has_value() const {
    return count() > 0;
  }
  void inc() {
    ++(*count_);
  }
  void dec() {
    --(*count_);
  }
  void destroy() {
    if (has_value()) {
      dec();
      if (!has_value()) {
        delete count_;
        delete ptr_;
        count_ = nullptr;
        ptr_ = nullptr;
      }
    }
  }
  pointer ptr_;
  counter *count_;
};
} // namespace cpp
#endif //CPP_HEADER_SHAREDPTR_H_

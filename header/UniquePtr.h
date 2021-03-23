#pragma once
#include <type_traits>

namespace cpp {
template <typename T>
class UniquePtr;

template <typename T, typename...Args>
UniquePtr<T> MakeUnique(Args&&...) noexcept(std::is_constructible<T, Args...>::value);

template <typename T>
class UniquePtr {
 public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;
  UniquePtr() noexcept : ptr_(nullptr) {}
  UniquePtr(UniquePtr &&other) noexcept {
    move(std::move(other));
  }
  UniquePtr(const_pointer ptr) noexcept : ptr_(ptr) {}
  UniquePtr &operator=(UniquePtr &&other) noexcept {
    Reset();
    move(std::move(other));
  }
  UniquePtr &operator=(const_pointer ptr) noexcept {
    Reset();
    ptr_ = ptr;
  }

  template <typename...Args>
  UniquePtr &Emplace(Args&&...args) noexcept(std::is_constructible<T, Args...>::value) {
    Reset();
    construct(std::forward<Args>(args)...);
  }

  bool HasValue() const noexcept { return ptr_ != nullptr; }

  pointer Get() noexcept { return ptr_; }
  const_pointer Get() const noexcept { return ptr_; }
  pointer operator->() noexcept { return Get(); }
  const_pointer operator->() const noexcept { return Get(); }
  reference operator*() noexcept { return &Get(); }
  const_reference operator*() const noexcept { return &Get(); }
  
  UniquePtr &Reset() {
    if (HasValue()) {
      delete ptr_;
      ptr_ = nullptr;
    }
    return *this;
  }

  ~UniquePtr() {
    Reset();
  }
 private:
  UniquePtr(const UniquePtr &) = delete;
  template <typename...Args>
  UniquePtr(Args&&...args) noexcept(std::is_constructible<T, Args...>::value) {
    construct(std::forward<Args>(args)...);
  }
  UniquePtr &operator=(const UniquePtr &) = delete;

  template <typename...Args>
  void construct(Args&&...args) noexcept(std::is_constructible<T, Args...>::value) {
    ptr_ = new T(std::forward<Args>(args)...);
  }

  void move(UniquePtr &&other) noexcept {
    if (other.HasValue()) {
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    } else {
      ptr_ = nullptr;
    }
  }

  template <typename T1_, typename...Args>
  friend UniquePtr<T1_> cpp::MakeUnique(Args&&...) noexcept(std::is_constructible<T1_, Args...>::value);
  T *ptr_;
};

template <typename T, typename ...Args>
UniquePtr<T> MakeUnique(Args&&...args) noexcept(std::is_constructible<T, Args...>::value) {
  return UniquePtr<T>(std::forward<Args>(args)...);
}
} // namespace cpp
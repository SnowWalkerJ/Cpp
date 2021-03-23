#pragma once
#include <type_traits>
#include <stdexcept>

namespace cpp {

namespace detail {
template <typename T>
struct TriviallyDestructibleStorage {
  constexpr TriviallyDestructibleStorage () : empty('\0'), has_value(false) {}
  void clear() { has_value = false; }
  union {
    char empty;
    T value;
  };
  bool has_value;
};
template <typename T>
struct NonTriviallyDestructibleStorage {
  NonTriviallyDestructibleStorage () : has_value(false) {}
  ~NonTriviallyDestructibleStorage () { clear(); }
  void clear() {
    if (has_value) {
      has_value = false;
      value.~T();
    }
  }
  union {
    char empty;
    T value;
  };
  bool has_value;
};

} // namespace detail

template <typename T>
class Optional {
 public:
  using value_type = T;
  using reference_type = value_type&;
  using const_reference = const value_type &;
  using pointer_type = value_type*;
  using const_pointer = const value_type*;

  static_assert(!std::is_abstract<T>::value, "Optional doesn't support abstract type");
  static_assert(!std::is_reference<T>::value, "Optional doesn't support reference type");

  constexpr Optional () noexcept {}

  Optional(const Optional &src) noexcept(std::is_nothrow_copy_constructible<value_type>::value) {
    if (src) {
      construct(*src);
    }
  }

  Optional(Optional &&src) noexcept(std::is_nothrow_move_constructible<value_type>::value) {
    if (src) {
      construct(std::move(*src));
      src.Reset();
    }
  }

  Optional(const value_type &value) noexcept(std::is_nothrow_copy_constructible<value_type>::value) {
    construct(value);
  }

  Optional(value_type &&value) noexcept(std::is_nothrow_move_constructible<value_type>::value) {
    construct(std::move(value));
  }

  template<typename...Args>
  reference_type Emplace(Args&&...args) {
    Reset();
    construct(std::forward<Args>(args)...);
    return *this;
  }

  const Optional &operator=(const Optional &other) noexcept(std::is_nothrow_copy_assignable<value_type>::value) {
    if (this != &other) {
      if (other) {
        assign(*other);
      } else {
        Reset();
      }
    }
    return *this;
  }

  const Optional &operator=(Optional &&other) noexcept(std::is_nothrow_move_assignable<value_type>::value) {
    if (this != &other) {
      if (other) {
        assign(std::move(*other));
        other.Reset();
      } else {
        Reset();
      }
    }
    return *this;
  }

  const Optional &operator=(const value_type &value) noexcept(std::is_nothrow_copy_assignable<value_type>::value) {
    assign(value);
    return *this;
  }

  const Optional &operator=(value_type &&value) noexcept(std::is_nothrow_move_assignable<value_type>::value) {
    assign(std::move(value));
    return *this;
  }

  constexpr explicit operator bool() const noexcept {
    return has_value();
  }

  reference_type operator*() {
    return value();
  }

  const value_type &operator*() const {
    return value();
  }

  pointer_type operator->() {
    require_value();
    return &storage_.value;
  }

  const_pointer operator->() const {
    require_value();
    return &storage_.value;
  }

  template<typename U>
  constexpr value_type ValueOr(U&& default_value) const& noexcept {
    if (has_value()) {
      return value();
    } else {
      return std::forward<U>(default_value);
    }
  }

  template<typename U>
  constexpr value_type ValueOr(U&& default_value) && noexcept {
    if (has_value()) {
      return std::move(value());
    } else {
      return std::forward<U>(default_value);
    }
  }

  constexpr bool HasValue() const noexcept { return has_value(); }

  const Optional &Reset() noexcept {
    storage_.clear();
  }

  pointer_type get_pointer() & {
    return has_value() ? &value() : nullptr;
  }

  const value_type *get_pointer() const& {
    return has_value() ? &value() : nullptr;
  }
  pointer_type get_pointer() && = delete;

 protected:
  template<typename ...Args>
  void construct(Args&&...args) {
    value_type *ptr = &storage_.value;
    new (ptr) value_type(std::forward<Args>(args)...);
    storage_.has_value = true;
  }

  void require_value() const {
    if (!has_value()) {
      throw std::runtime_error("acessing an optional without a value");
    }
  }

  void assign(const value_type &value) {
    if (has_value()) {
      storage_.value = value;
    } else {
      construct(value);
    }
  }

  void assign(value_type &&value) {
    if (has_value()) {
      storage_.value = std::move(value);
    } else {
      construct(std::move(value));
    }
  }

  constexpr reference_type value() & {
    require_value();
    return storage_.value;
  }

  constexpr const_reference value() const& {
    require_value();
    return storage_.value;
  }

  constexpr value_type &&value() && {
    require_value();
    return std::move(storage_.value);
  }

  constexpr const value_type &&value() const&& {
    require_value();
    return std::move(storage_.value);
  }

  constexpr bool has_value() const noexcept {
    return storage_.has_value;
  }

 private:
  using Storage = typename std::conditional<std::is_trivially_destructible<value_type>::value,
                                            detail::TriviallyDestructibleStorage<value_type>,
                                            detail::NonTriviallyDestructibleStorage<value_type> >::type;
  Storage storage_;
};

} // namespace cpp
#include <type_traits>
#include "Optional.h"

namespace cpp {
namespace detail {
template <typename F>
class Lazy {
 public:
  using result_type = typename std::result_of<F()>::type;
  static_assert(!std::is_reference<F>::value, "Func should not be a reference type");
  static_assert(!std::is_const<F>::value, "Func should not be a const-qualified type");

  explicit Lazy(F &&f) : func_(std::move(f)) {}
  explicit Lazy(const F &f) : func_(f) {}
  Lazy(const Lazy &) = delete;
  Lazy(Lazy &&other) : value_(std::move(other.value_)), func_(std::move(other.func_)) {}

  Lazy &operator=(const Lazy &) = delete;
  Lazy &operator=(Lazy &&) = delete;

  operator result_type() const {
    return *(*this);
  }

  result_type &operator *() & {
    ensure_value();
    return *value_;
  }

  const result_type &operator *() const& {
    ensure_value();
    return *value_;
  }

  result_type &&operator *() && {
    ensure_value();
    return std::move(*value_);
  }

  result_type *operator ->() {
    ensure_value();
    return value_;
  }

  const result_type *operator ->() const {
    ensure_value();
    return value_;
  }

 protected:
  void ensure_value() const {
    if (!has_value()) {
      value_ = func_();
    }
  }

  constexpr bool has_value() const noexcept { return value_.HasValue(); }

 private:
  mutable Optional<result_type> value_;
  F func_;
};
} // namespace detail

template <typename Func>
detail::Lazy<typename std::decay<Func>::type> lazy(Func &&func) {
  return detail::Lazy<typename std::decay<Func>::type>(std::forward<Func>(func));
}

} // namespace cpp
//
// Created by 陈秉烜 on 2021/3/22.
//

#ifndef CPP_HEADER_VARIANT_H_
#define CPP_HEADER_VARIANT_H_
#include "Optional.h"
namespace cpp {
namespace detail {
template <typename Type>
struct variant_ctor {
};

template<typename...Types>
struct variant_impl;

template<typename Type>
struct variant_impl<Type> : variant_ctor<Type> {
  variant_impl(const Type &);
  variant_impl(Type &&);
};

template<typename Type, typename ...Rest>
struct variant_impl<Type, Rest...> : variant_ctor<Type>, variant_ctor<Rest>... {
  template <typename T, typename=std::enable_if_t<std::is_base_of_v<variant_impl<Type, Rest...>, variant_ctor<T> > > >
  variant_impl(const T &value) {}
};

} // namespace detail

template <typename...Types>
class Variant {
 public:
  explicit constexpr Variant() noexcept : impl_() {}
  template <typename T>
  explicit Variant(T &&value) : impl_(std::forward<T>(value)) {}
 private:
  detail::variant_impl<Types...> impl_;
};
} // namespace cpp
#endif //CPP_HEADER_VARIANT_H_

#ifndef MATRIX2D_INDEX_HPP
#define MATRIX2D_INDEX_HPP

#include <cstdint>
#include <ostream>
#include <tuple>

namespace matrix2d {

struct Index {
  using value_type = std::uint_fast32_t;
  // Default cons constexpr https://stackoverflow.com/a/36392448/8594193
  value_type y = 0;
  value_type x = 0;
  constexpr Index() = default;
  constexpr Index(value_type y_arg, value_type x_arg) : y(y_arg), x(x_arg) {}
  constexpr Index(const Index&) = default;
  constexpr Index& operator=(const Index&) = default;
};

inline constexpr bool operator<(const Index i0, const Index i1) {
  return std::forward_as_tuple(i0.y, i0.x) < std::forward_as_tuple(i1.y, i1.x);
}

inline constexpr bool operator<=(const Index i0, const Index i1) {
  return std::forward_as_tuple(i0.y, i0.x) <= std::forward_as_tuple(i1.y, i1.x);
}

inline constexpr bool operator>(const Index i0, const Index i1) {
  return std::forward_as_tuple(i0.y, i0.x) > std::forward_as_tuple(i1.y, i1.x);
}

inline constexpr bool operator>=(const Index i0, const Index i1) {
  return std::forward_as_tuple(i0.y, i0.x) >= std::forward_as_tuple(i1.y, i1.x);
}

inline constexpr bool operator==(const Index i0, const Index i1) {
  return std::forward_as_tuple(i0.y, i0.x) == std::forward_as_tuple(i1.y, i1.x);
}

inline constexpr bool operator!=(const Index i0, const Index i1) {
  return !(i0 == i1);
}

inline std::ostream& operator<<(std::ostream& os, const Index& index) {
  return os << "[" << index.y << ", " << index.x << "]";
}

} // namespace matrix2d

#endif // MATRIX2D_INDEX_HPP

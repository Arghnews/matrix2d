#ifndef MATRIX2D_TPP
#define MATRIX2D_TPP

#include "matrix2d/matrix2d.hpp"

#include <boost/iterator/iterator_facade.hpp>

#include <range/v3/action/join.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/cache1.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// https://github.com/fmtlib/fmt/issues/751
// https://github.com/fmtlib/fmt/pull/759
namespace fmt {
template <class T, class Char>
struct is_range<matrix2d::Matrix2d<T>, Char> : std::false_type {};
} // namespace fmt

namespace matrix2d {

// NOTE: Do NOT follow the boost docs and use
// boost::random_access_traversal_tag as the iterator category tag for row
// iterators. If you do this, then call say, std::distance, std::distance will
// check to see if there iterator_traits::iterator_category is
// std::random_access_iterator_tag, which it isn't as it's
// boost::random_access_traversal_tag.
// This means that std::distance will assume the row_iterator is an input
// iterator, and will keep calling ++row_iterator until you hit the end. If
// you called std::distance(x, y) where y is before x, then you're screwed (x
// will be inced till it hits end_, or without end_ into UB past end_).
// Whereas with the correct iterator_category distance_to will be called as
// expected.
// This was annoying to find to say the least.

// https://www.boost.org/doc/libs/1_72_0/libs/iterator/doc/iterator_facade.html
// "Should" be the size_type of the underlying ContiguousContainer
// using size_type = std::size_t;
// using difference_type = std::ptrdiff_t;
// typename std::iterator_traits<Iterator>::difference_type;

template <class V>
template <class I, class VT>
template <class I1, class VT1, class>
Matrix2d<V>::row_iter<I, VT>::row_iter(const row_iter<I1, VT1>& other)
    : row_(other.row_), end_(other.end_), size_(other.size_) {}

template <class V>
template <class I, class VT>
Matrix2d<V>::row_iter<I, VT>::row_iter(const Iterator row, const size_type size,
                                       const Iterator end)
    : row_(row), end_(end), size_(size) {}

template <class V>
template <class I, class VT>
void Matrix2d<V>::row_iter<I, VT>::advance(const difference_type n) {
  if (n > 0) {
    assert(row_ < end_ && "row_ iterator advanced past end_");
  }
  std::advance(row_, static_cast<difference_type>(size_) * n);
}

template <class V>
template <class I, class VT>
void Matrix2d<V>::row_iter<I, VT>::increment() {
  this->advance(1);
}

template <class V>
template <class I, class VT>
void Matrix2d<V>::row_iter<I, VT>::decrement() {
  this->advance(-1);
}

template <class V>
template <class I, class VT>
template <class I1, class VT1>
typename Matrix2d<V>::difference_type //
Matrix2d<V>::row_iter<I, VT>::distance_to(
    const row_iter<I1, VT1>& other) const {
  assert(size_ == other.size_);
  if (size_ == 0) {
    // Must be a zero sized matrix
    assert(row_ == other.row_);
    return 0;
  }
  static_assert(
      std::is_signed_v<decltype((other.row_ - row_) /
                                static_cast<difference_type>(size_))>);
  return (other.row_ - row_) / static_cast<difference_type>(size_);
}

template <class V>
template <class I, class VT>
template <class I1, class VT1>
bool Matrix2d<V>::row_iter<I, VT>::equal(const row_iter<I1, VT1>& other) const {
  assert(size_ == other.size_);
  return this->row_ == other.row_;
}

// dereference should return a reference type, we're using a span so the
// valuetype is a view onto the data
template <class V>
template <class I, class VT>
VT Matrix2d<V>::row_iter<I, VT>::dereference() const {
  assert(row_ < end_);
  // assert(row_ + size_ <= end_); // Could push iterator/pointer past end_
  assert(end_ - row_ >= static_cast<difference_type>(size_));
  static_assert(std::is_lvalue_reference_v<decltype(*row_)>);
  return {&*row_, size_};
}


// NOTE: can't (easily anyway) use ranges::subrange in place of this class.
// It stores two iterators (well iterator+sentinel) (call them first, last),
// not an iterator+size_ pair like this. This means for a loop like
// for (auto it = v.begin(); it != v.end(); ++it)
// on the last iteration, first == v.end(), but last == first + size_ ie. we
// will increment past the end iterator which is UB
// Whereas this will have row_ == v.end() and size_ is just a number.

template <class V>
std::string Matrix2d<V>::to_string(const std::string_view elem_delim,
                                       const std::string_view row_delim,
                                       const std::size_t pad) const {
  if (this->empty()) {
    return "";
  }

  const auto width =
      pad + ranges::max(ranges::views::all(*this) |
                        ranges::views::transform([](const auto& elem) {
                          return fmt::formatted_size("{}", elem);
                        }));

  // Must call the below range pipeline to produce a string on an lvalue. Since
  // we end the pipeline with to_string everything is evaluated here, so no
  // lifetime issue.
  auto rows = this->rows_iter();

  // See dynamic width https://fmt.dev/7.1.3/syntax.html#format-examples
  return rows | ranges::views::transform([elem_delim, width](const auto& row) {
           // https://old.reddit.com/r/cpp/comments/opuu92/whats_up_with_c_ranges/h68o4ey/
           return fmt::format("{:>{}}", fmt::join(row, elem_delim), width);
         }) |
         ranges::views::cache1 | ranges::views::join(row_delim) |
         ranges::to<std::string>();
}

template <class V>
std::ostream& operator<<(std::ostream& os, const Matrix2d<V>& m) {
  return os << m.to_string();
}

template <class V>
inline Matrix2d<V>::Matrix2d(const size_type rows, const size_type cols)
    : rows_(rows), cols_(cols), data_(rows * cols) {
  // std::fill(data_.begin(), data_.end(), value_type{});
}

template <class V>
inline typename Matrix2d<V>::value_type
Matrix2d<V>::operator()(const size_type row, const size_type col) const {
#ifndef NDEBUG
  if (row >= rows_ || col >= cols_)
    throw std::out_of_range("const Matrix2d subscript out of bounds");
#endif
  return data_[cols_ * row + col];
}

template <class V>
inline typename Matrix2d<V>::reference
Matrix2d<V>::operator()(const size_type row, const size_type col) {
#ifndef NDEBUG
  if (row >= rows_ || col >= cols_)
    throw std::out_of_range("const Matrix2d subscript out of bounds");
#endif
  return data_[cols_ * row + col];
}

template <class V>
inline typename Matrix2d<V>::value_type
Matrix2d<V>::operator()(const Index& index) const {
  return this->operator()(index.y, index.x);
}

template <class V>
inline typename Matrix2d<V>::reference
Matrix2d<V>::operator()(const Index& index) {
  return this->operator()(index.y, index.x);
}

} // namespace matrix2d

#endif // MATRIX2D_TPP

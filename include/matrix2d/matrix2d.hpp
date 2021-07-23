#ifndef MATRIX2D_HPP
#define MATRIX2D_HPP

#include "matrix2d/index.hpp"

#include "boost/container/small_vector.hpp"
#include "boost/iterator/iterator_facade.hpp"
#include "nonstd/span.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ostream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// Simple 2D matrix class. Contiguous storage. Row major order.
// Preset size specified on construction at runtime. Access via pair like index
// or matrix(3, 4). May get an iterator over rows.

// If being pedantic could shove constexpr/noexcept (almost) everywhere.
// Uses boost iterator facade as per
// https://www.boost.org/doc/libs/1_72_0/libs/iterator/doc/iterator_facade.html

// TODO: remove
#include "fmt/format.h"

namespace matrix2d {

// Would like this, but std::vector<Index>{}.emplace_back(y, x) won't work
// without a constructor
// https://stackoverflow.com/a/43395131/8594193
// static_assert(std::is_aggregate_v<Index>);

// TODO: change Matrix2d operator() to be unchecked when on release mode,
// add .at() alternative Add init list constructors? Urgh. Rename to
// 2DMatrix2d
// https://isocpp.org/wiki/faq/operator-overloading#matrix-subscript-op

template <class Val> class Matrix2d {

  // Would like to be able to use with matrix of bools. std::vector<bool>
  // specialisation kicks in, causes all kinds of issues due to the proxy
  // objects, so avoid std::vector as underlying
  // Hard coded small vector value as don't really want to have to change entire
  // class interface for that.
  // using ContiguousContainer =
  // boost::container::small_vector<Val, (sizeof(Val) > 32 ? 8 : 64)>;
  using ContiguousContainer = boost::container::small_vector<Val, 8>;
  // using ContiguousContainer = std::vector<Val>;

public:
  using value_type = typename ContiguousContainer::value_type;
  using allocator_type = typename ContiguousContainer::allocator_type;
  using size_type = typename ContiguousContainer::size_type;
  using difference_type = typename ContiguousContainer::difference_type;
  using reference = typename ContiguousContainer::reference;
  using const_reference = typename ContiguousContainer::const_reference;
  using pointer = typename ContiguousContainer::pointer;
  using const_pointer = typename ContiguousContainer::const_pointer;

private:
  size_type rows_, cols_;
  ContiguousContainer data_;

public:
  explicit Matrix2d(size_type rows, size_type cols);

  // Subscript operators often come in pairs
  value_type operator()(const Index& index) const;
  reference operator()(const Index& index);
  value_type operator()(size_type row, size_type col) const;
  reference operator()(size_type row, size_type col);

  Matrix2d(const Matrix2d& m) = default;
  Matrix2d& operator=(const Matrix2d& m) = default;
  Matrix2d(Matrix2d&& m) = delete;
  Matrix2d& operator=(Matrix2d&& m) = delete;

  using iterator = typename ContiguousContainer::iterator;
  using const_iterator = typename ContiguousContainer::iterator;

  // https://www.boost.org/doc/libs/1_72_0/libs/iterator/doc/iterator_facade.html
  template <class Iter, class ValueType>
  class row_iter
      : public boost::iterator_facade<row_iter<Iter, ValueType>, ValueType,
                                      std::random_access_iterator_tag,
                                      ValueType> {

    // using Parent =
    // boost::iterator_facade<row_iter<Iter, ValueType>, ValueType,
    // std::random_access_iterator_tag, ValueType>;
    // using typename Parent::size_type;
    using Iterator = Iter;
    // using size_type = size_type;
    friend class boost::iterator_core_access;
    template <class> friend class Matrix2d;

  public:
    // using difference_type = difference_type;
    // using typename Parent::difference_type;

    row_iter() = default;

    template <
        class Other, class Value,
        typename = std::enable_if_t<std::is_convertible_v<Other, Iterator>>>
    row_iter(const row_iter<Other, Value>& other);

  private:
    explicit row_iter(const Iterator row, const size_type size,
                      const Iterator end);
    void advance(const difference_type n);
    void increment();
    void decrement();

    template <class Other, class Value>
    difference_type distance_to(const row_iter<Other, Value>& other) const;

    template <class Other, class Value>
    bool equal(const row_iter<Other, Value>& other) const;

    ValueType dereference() const;

    Iterator row_;
    Iterator end_;
    size_type size_;
  };

  using rows_iterator = row_iter<typename ContiguousContainer::iterator,
                                 nonstd::span<value_type>>;
  using rows_const_iterator =
      row_iter<typename ContiguousContainer::const_iterator,
               nonstd::span<const value_type>>;

  auto begin() { return data_.begin(); }
  auto begin() const { return data_.begin(); }
  auto end() { return data_.end(); }
  auto end() const { return data_.end(); }

  size_type size() const { return data_.size(); }
  bool empty() const { return data_.empty(); }

  template <class T> struct rows_iterable_wrapper {
    T iterable;
    auto begin() { return iterable.rows_begin(); }
    auto begin() const { return iterable.rows_begin(); }
    auto end() { return iterable.rows_end(); }
    auto end() const { return iterable.rows_end(); }
  };

public:
  auto rows_iter() & { return rows_iterable_wrapper<Matrix2d&>{*this}; }
  auto rows_iter() const& {
    return rows_iterable_wrapper<const Matrix2d&>{*this};
  }
  auto rows_iter() && {
    return rows_iterable_wrapper<Matrix2d>{std::move(*this)};
  }

  auto rows_begin() { return rows_iterator(data_.begin(), cols_, data_.end()); }
  auto rows_begin() const {
    return rows_const_iterator(data_.begin(), cols_, data_.end());
  }
  auto rows_end() { return rows_iterator(data_.end(), cols_, data_.end()); }
  auto rows_end() const {
    return rows_const_iterator(data_.end(), cols_, data_.end());
  }

  size_type rows() const { return rows_; }
  size_type columns() const { return cols_; }

  // Here temporarily as fmtlib doesn't catch the ostream operator<< overload
  // I've defined for my matrix class and instead uses the default fmt/ranges as
  // my matrix has begin/end so qualifies as a range, and prints the whole damn
  // matrix as a one liner. Unsure how to precisely fix this. Can just duplicate
  // and specify overload for fmtlib specifically for matrix

  std::string to_string(std::string_view elem_delim = ",",
                        std::string_view row_delim = "\n",
                        std::size_t pad = 1) const;

};

template <class T>
std::ostream& operator<<(std::ostream&, const Matrix2d<T>&);
} // namespace matrix2d

#include "matrix2d/matrix2d.tpp"

#endif // MATRIX2D_HPP

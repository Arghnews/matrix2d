#include "matrix2d/matrix2d.hpp"

#include "catch2/catch.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "range/v3/algorithm/equal.hpp"
#include "range/v3/numeric/iota.hpp"
#include "range/v3/view/all.hpp"
#include "range/v3/view/chunk.hpp"
#include "range/v3/view/zip.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <vector>

TEST_CASE("Empty", "[]") {
  matrix2d::Matrix2d<float> m{0, 0};
  REQUIRE(m.size() == 0);
  REQUIRE(m.empty());
  REQUIRE(m.rows() == 0);
  REQUIRE(m.columns() == 0);
  REQUIRE(m.rows_begin() == m.rows_end());
  REQUIRE(m.columns() == 0);
  REQUIRE(m.begin() == m.end());
  REQUIRE(std::distance(m.begin(), m.end()) == 0);
  REQUIRE_NOTHROW(fmt::format("{}", m));
}

TEST_CASE("Square", "[]") {
  matrix2d::Matrix2d<float> m{3, 3};
  REQUIRE(m.size() == 9);
  REQUIRE(!m.empty());
  REQUIRE(m.rows() == 3);
  REQUIRE(m.columns() == 3);
  REQUIRE(m.rows_begin() != m.rows_end());
  REQUIRE(m.columns() == 3);
  REQUIRE(m.begin() != m.end());
  REQUIRE(std::distance(m.begin(), m.end()) == 9);

  ranges::iota(m, 0);

  std::vector<float> v(9);
  ranges::iota(v, 0);

  const auto v0 = ranges::views::all(v) | ranges::views::chunk(3);
  const auto v1 = m.rows_iter();

  for (const auto [vec_row, matrix_row] : ranges::views::zip(v0, v1)) {
    REQUIRE(ranges::equal(vec_row, matrix_row));
  }
  REQUIRE_NOTHROW(fmt::format("{}", m));
}

TEST_CASE("Matrix", "[]") {
  // 5 rows, 3 cols
  matrix2d::Matrix2d<float> m{5, 3};

  auto i = 0UL;
  for (auto row : m.rows_iter()) {
    ranges::iota(row, i);
    i += row.size();
  }

  fmt::print("{}\n", m);
  REQUIRE_NOTHROW(fmt::format("{}", m));
  const auto i0 = m.rows_begin();
  auto i1 = std::next(m.rows_begin());
  const auto i5 = m.rows_end();

  REQUIRE(i0 < i1);
  REQUIRE(i0 < i5);
  REQUIRE(i1 < i5);

  REQUIRE((i1 - i0) == std::distance(i0, i1));
  REQUIRE((i0 - i1) == std::distance(i1, i0));
  // fmt::print("{}\n", i0.distance_to(i1));
  // fmt::print("{}\n", i1.distance_to(i0));
  // fmt::print("{}\n", std::distance(i0, i1));
  // fmt::print("{}\n", std::distance(i1, i0));

  REQUIRE(i1 > i0);
  REQUIRE(i5 > i0);
  REQUIRE(i5 > i1);

  REQUIRE(i0 != i1);
  REQUIRE(i0 != i1);

  REQUIRE(i0 == i0);
  REQUIRE(i0 == m.rows_begin());

  REQUIRE(i5 == i5);
  REQUIRE(i5 == m.rows_end());
}

TEST_CASE("Matrix iterator types", "[]") {
  // 5 rows, 3 cols
  matrix2d::Matrix2d<float> m{5, 3};
  const auto& cm = m;

  REQUIRE(cm.begin() == m.begin());
  REQUIRE(cm.end() == m.end());
  REQUIRE(cm.rows_begin() == m.rows_begin());
  REQUIRE(cm.rows_end() == m.rows_end());

  REQUIRE(cm.rows_begin() <= m.rows_end());
  REQUIRE_FALSE(cm.rows_begin() > m.rows_end());

  using RowsIterator = decltype(m)::rows_iterator;
  using RowsIteratorConst = decltype(m)::rows_const_iterator;

  static_assert(std::is_same_v<decltype(m.rows_begin()), RowsIterator>);
  static_assert(std::is_same_v<decltype(cm.rows_begin()), RowsIteratorConst>);

  static_assert(std::is_convertible_v<RowsIterator, RowsIteratorConst>);
  static_assert(!std::is_convertible_v<RowsIteratorConst, RowsIterator>);

  static_assert(std::is_constructible_v<RowsIterator, RowsIterator>);
  static_assert(std::is_constructible_v<RowsIteratorConst, RowsIteratorConst>);

  static_assert(std::is_constructible_v<RowsIteratorConst, RowsIterator>);
  static_assert(!std::is_constructible_v<RowsIterator, RowsIteratorConst>);
}

TEST_CASE("Matrix rows_iter", "[]") {
  matrix2d::Matrix2d<int> m{3, 2};
  const matrix2d::Matrix2d<int> cm{3, 2};

  for (auto row : m.rows_iter()) {
    row.front() = 1;
  }
  REQUIRE(m(0, 0) == 1);
  m(0, 0) = 5;
  REQUIRE(m(0, 0) == 5);

  for (auto row : cm.rows_iter()) {
    static_assert(std::is_const_v<std::remove_reference_t<decltype(row[0])>>);
  }
}

TEST_CASE("Test contiguous", "[]") {
  matrix2d::Matrix2d<int> m{5, 3}; // 5 rows, 3 elements per row
  auto rows_it = m.rows_begin();
  auto end_of_first_row = rows_it->end();

  const auto last_element_of_first_row = std::prev(end_of_first_row);
  const auto first_of_second_row = std::next(m.rows_begin())->begin();

  REQUIRE(reinterpret_cast<char*>(&*last_element_of_first_row) +
              sizeof(decltype(m)::value_type) ==
          reinterpret_cast<char*>(&*first_of_second_row));
  REQUIRE(m.data() == &*m.begin());
  REQUIRE(m.data() == m.rows_begin()->data());
}

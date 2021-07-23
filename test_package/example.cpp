#include "matrix2d/matrix2d.hpp"

#include <iostream>
#include <numeric>
#include <sstream>

int main() {
  matrix2d::Matrix2d<int> m{3, 3};
  std::iota(m.begin(), m.end(), 0);
  for (auto row : m.rows_iter()) {
    row.front() = row.front() * 2;
  }

  m(2, 2) = 44;
  m({0, 0}) = 100;
  std::cout << "See matrix2d:\n" << m << "\n";
  fmt::print("See simple mat:\n{}\n", m);

  for (auto row : m.rows_iter()) {
    std::ostringstream ss{};
    for (const auto elem : row) {
      ss << elem << " ";
    }
    std::cout << ss.str() << "\n";
  }

  std::cout << m.to_string(",", "\n", 0) << "\n";
}

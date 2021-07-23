//#include "prettyprint.hpp"
//#include <fmt/ranges.h>
//#include <fmt/ostream.h>
//#include <range/v3/all.hpp>

#include "matrix2d/matrix2d.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

int main(int /*argc*/, char** /*argv*/)
{
  {
    matrix2d::Matrix2d<float> sm{3, 3};
    sm(2, 1) = 5;
    std::cout << sm << "\n";
    fmt::print("{}\n", sm);
  }

  {
    matrix2d::Matrix2d<bool> sm{3, 3};
    std::cout << sm << "\n";
  }
  // std::cout << sm.size() << "\n";
  // for (const auto& row: sm.rows_iter())
  // {
    // fmt::print("{}\n", row);
  // }
  // fmt::print("sm.to_string: {}\n", sm.to_string());
  // std::cout << sm.to_string() << "\n";
  // std::cout << std::boolalpha;
  // std::cout << "Hello world!" << "\n";
}

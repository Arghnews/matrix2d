## Simple row major contiguous matrix
- Row iteration
- Not resizable, size must be specified via arguments at construction
- Requires c++17

Tested with clang 10 and gcc 8
Intended to be consumed via the [conan package manager](https://conan.io/) and cmake

### Use via conan
- Add the conan repo where this package is hosted as a remote, replacing <NAME> with something appropriate
    * Run `conan remote add <NAME> https://arghnews.jfrog.io/artifactory/api/conan/arghnews-conan`
- Add to your project's conan requires "matrix2d/[>=0.2.6]@arghnews/testing"

Then after conan install, use as normal with cmake find_package
```cmake
find_package(matrix2d REQUIRED)
...
target_link_libraries(myproject PRIVATE matrix2d::matrix2d)
```

It should be possible to use without conan, as long as on your system you have the libraries this program depends on, as shown in the requies section of conanfile.py

```cpp
#include "matrix2d/matrix2d.hpp"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <numeric>

int main() {
  matrix2d::Matrix2d<std::size_t> m{5, 3}; // 5 rows, 3 elements per row
  assert(m.rows() == 5);
  assert(m.columns() == 3);
  assert(!m.empty());

  // Matrix is contiguous (row major), fill with values from 1 -> size
  std::iota(m.begin(), m.end(), 1);

  // 0 indexed, (y, x) - yes this may be confusing, take note
  // m(2, 1) is the third row, second element in that row
  assert(m(4, 2) == m.size());

  std::cout << m << "\n";
  //  1,  2,  3
  //  4,  5,  6
  //  7,  8,  9
  // 10, 11, 12
  // 13, 14, 15

  // with fmt/format.h
  // fmt::print("{}\n", m);

  // Rows are view objects onto the actual rows
  for (auto row : m.rows_iter()) {
    row.front() = 100; // Set first element in each row to 100

    // If using fmtlib ranges.h helper
    // fmt::print("Row: {}\n", row);

  }

  assert(m.rows_begin()->front() == 100);
  assert((--m.rows_end())->front() == 100);

  for (const auto& ele : m) {
    std::cout << ele << " ";
  }
  std::cout << "\n";
  // 100 2 3 100 5 6 100 8 9 100 11 12 100 14 15
}
```

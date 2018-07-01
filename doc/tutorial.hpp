// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

/*!
@mainpage User Manual



@tableofcontents



@section Description

------------------------------------------------------------------------------
Extension of the standard library.



@section Installation

------------------------------------------------------------------------------
Since Umlaut is a header-only library all you need to do is download the source code
from the GitHub [repository][umlaut.repository] and add the `include/` directory to your
compiler's library search path. However, if your project uses CMake is can be beneficial
to install the library using CMake.

From the root directory of the project execute the
following commands to install:

@code{.sh}
mkdir build && cd build
cmake ..
cmake --build . --target install
@endcode

Umlaut can then be used in any of your CMake project using `find_package(Umlaut)`.\n
Below is a minimal example of how this could look:

@code{cmake}
cmake_minimum_required(VERSION 3.0)
project(test_project)

find_package(Umlaut REQUIRED)

add_executable(test_project main.cpp)
target_link_libraries(test_project Umlaut::Umlaut)
@endcode



<!-- Links -->
[Umlaut.repository]: https://github.com/MaLarsson/umlaut

*/

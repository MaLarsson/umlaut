// Copyright Marcus Larsson 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <umlaut.hpp>

namespace {

TEST(smallVectorBase, listInitialization) {
    umlaut::small_vector_base<int> v(umlaut::list_construct, 1, 2, 3);
}

} // namespace

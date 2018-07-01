#include <gtest/gtest.h>
#include <umlaut.hpp>

TEST(smallVectorBase, listInitialization) {
    umlaut::small_vector_base<int> v(umlaut::list_construct, 1, 2, 3);
}

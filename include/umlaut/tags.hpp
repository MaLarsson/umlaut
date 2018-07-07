/// @file
/// Defines common tag type.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

namespace umlaut {

/// @brief Disambiguator tag.
///
/// Empty struct tag type used to disambiguate between different functions
/// taking a list of objects for when initializing a container.
/// This is used as an alternative to `std::initializer_list` but without the overhead
/// of `std::initializer_list`. For more information see the
/// <a href="https://www.youtube.com/watch?v=sSlmmZMFsXQ">presentation</a>
/// by Jason Turner at C++Now 2018.
struct list_construct_t { explicit list_construct_t() = default; };

/// @relates list_construct_t
/// @brief Instance of the disambiguator tag umlaut::list_construct_t.
inline constexpr list_construct_t list_construct = list_construct_t{};

} // namespace umlaut

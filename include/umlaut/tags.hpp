/// @file
/// Defines common tag type.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

namespace umlaut {

struct list_construct_t { explicit list_construct_t() = default; };

inline constexpr list_construct_t list_construct = list_construct_t{};

} // namespace umlaut

/// @file
/// Defines configuration macros.
///
/// @copyright Marcus Larsson 2018
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#if defined(__has_builtin)
#define UMLAUT_HAS_BUILTIN(x) __has_builtin(x)
#else
#define UMLAUT_HAS_BUILTIN(x) 0
#endif

#if UMLAUT_HAS_BUILTIN(__builtin_expect)
#define UMLAUT_UNLIKELY(x) (__builtin_expect(x, 0))
#define UMLAUT_LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define UMLAUT_UNLIKELY(x) (x)
#define UMLAUT_LIKELY(x) (x)
#endif

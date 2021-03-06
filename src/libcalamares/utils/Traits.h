/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   SPDX-FileCopyrightText: 2020 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef UTILS_TRAITS_H
#define UTILS_TRAITS_H

#include <type_traits>


namespace CalamaresUtils
{

/** @brief Traits machinery lives in this namespace
 *
 * The primary purpose of this namespace is to hold machinery that
 * is created by the DECLARE_HAS_METHOD macro.
 *
 * The DECLARE_HAS_METHOD macro builds machinery to check whether
 * a class has a particular named method. This can be used to
 * specialize templates elsewhere for use with classes with, or without,
 * the named method.
 *
 * To use the machinery (which is not that sophisticated):
 *
 *  - Put `DECLARE_HAS_METHOD(myFunction)` somewhere in file scope.
 *    This puts together the machinery for detecting if `myFunction`
 *    is a method of some class.
 *  - At global scope, `has_myFunction<T>` is now either std::true_type,
 *    if the type `T` has a method `T::myFunction`, or std::false_type,
 *    if it does not.
 *
 * To specialize template methods based on the presence of the named
 * method, write **three** overloads:
 *
 *  - `template<class T> myMethod(args ..., const std::true_type& )`
 *    This is the implementation where class T has `myFunction`.
 *  - `template<class T> myMethod(args ..., const std::false_type& )`
 *    This is the implementation without.
 *  - `template<class T> myMethod(args ...)` is the general implementation,
 *    which can call the specialized implementations with
 *    `return myMethod(args ..., has_myFunction<T>{})`
 */
namespace Traits
{
template< class > struct sfinae_true : std::true_type{};
}
}

#define DECLARE_HAS_METHOD(m) \
    namespace CalamaresUtils { namespace Traits { \
    struct has_ ## m { \
    template< class T > static auto f(int) -> sfinae_true<decltype(&T:: m)>; \
    template< class T > static auto f(long) -> std::false_type; \
    template< class T > using t = decltype( f <T>(0) ); \
    }; } } \
    template< class T > using has_ ## m = CalamaresUtils::Traits:: has_ ## m ::t<T>;

#endif

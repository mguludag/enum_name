/*
MIT License

Copyright (c) 2024 mguludag

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef MGUTILITY_COMMON_DEFINITIONS_HPP
#define MGUTILITY_COMMON_DEFINITIONS_HPP

/**
 * @file definitions.hpp
 * @brief Defines macros for compiler and standard support detection.
 */

/**
 * @brief Defines the MGUTILITY_CPLUSPLUS macro for MSVC and other compilers.
 *
 * For MSVC, it uses _MSVC_LANG. For other compilers, it uses __cplusplus.
 */
#ifdef _MSC_VER
#define MGUTILITY_CPLUSPLUS _MSVC_LANG
#else
#define MGUTILITY_CPLUSPLUS __cplusplus
#endif

/**
 * @brief Defines the MGUTILITY_CNSTXPR macro based on the C++ standard.
 *
 * If the C++ standard is C++11, MGUTILITY_CNSTXPR is defined as empty.
 * If the C++ standard is newer than C++11, MGUTILITY_CNSTXPR is defined as
 * constexpr. If the C++ standard is older than C++11, an error is raised.
 */
#if MGUTILITY_CPLUSPLUS == 201103L
#define MGUTILITY_CNSTXPR
#elif MGUTILITY_CPLUSPLUS > 201103L
#define MGUTILITY_CNSTXPR constexpr
#elif MGUTILITY_CPLUSPLUS < 201103L
#error "Standards older than C++11 is not supported!"
#endif

/**
 * @brief Defines the MGUTILITY_CNSTXPR_CLANG_WA macro based on the C++
 * standard.
 *
 * If the C++ standard is newer than C++17 and the compiler is not Clang,
 * MGUTILITY_CNSTXPR_CLANG_WA is defined as constexpr. Otherwise, it is defined
 * as empty.
 */
 #if (MGUTILITY_CPLUSPLUS >= 201703L  && !defined(__clang__)) || (defined(__clang__) && __clang_major__ > 11 && MGUTILITY_CPLUSPLUS >= 201703L)
#define MGUTILITY_CNSTXPR_CLANG_WA constexpr
#else
#define MGUTILITY_CNSTXPR_CLANG_WA
#endif


/**
 * @brief Defines the MGUTILITY_CNSTEVL macro based on the C++ standard.
 *
 * If the C++ standard is newer than C++17, MGUTILITY_CNSTEVL is defined as
 * consteval. Otherwise, it is defined as empty.
 */
#if MGUTILITY_CPLUSPLUS > 201703L
#define MGUTILITY_CNSTEVL consteval
#else
#define MGUTILITY_CNSTEVL
#endif

#endif // MGUTILITY_COMMON_DEFINITIONS_HPP

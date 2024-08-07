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

#ifndef DETAIL_ENUM_FOR_EACH_HPP
#define DETAIL_ENUM_FOR_EACH_HPP

#include "meta.hpp"
#include "mgutility/std/string_view.hpp"

#include <cstdint>
#include <utility>

namespace mgutility {
namespace detail {
/**
 * @brief Alias template for a string or string view type based on the presence
 * of a bitwise OR operator.
 *
 * If the type T supports the bitwise OR operator, the alias is a std::string.
 * Otherwise, it is a mgutility::string_view.
 *
 * @tparam T The type to check.
 */
template <typename T>
using string_or_view_t =
    typename std::conditional<has_bit_or<T>::value, std::string,
                              mgutility::string_view>::type;

/**
 * @brief A pair consisting of an enum value and its corresponding string or
 * string view.
 *
 * @tparam Enum The enum type.
 */
template <typename Enum>
using enum_pair = std::pair<Enum, detail::string_or_view_t<Enum>>;
} // namespace detail

/**
 * @brief A class template for iterating over enum values.
 *
 * @tparam Enum The enum type.
 */
template <typename Enum> class enum_for_each {
  using value_type = const detail::enum_pair<Enum>;
  using size_type = std::size_t;

  /**
   * @brief An iterator for enum values.
   */
  struct enum_iter {
    using const_iter_type = decltype(enum_range<Enum>::min);
    using iter_type = detail::remove_const_t<const_iter_type>;
    using iterator_category = std::forward_iterator_tag;
    using value_type = const detail::enum_pair<Enum>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    /**
     * @brief Default constructor initializing the iterator to the default
     * position.
     */
    enum_iter() : m_pos{} {}

    /**
     * @brief Constructor initializing the iterator to a specific position.
     *
     * @param value The initial position of the iterator.
     */
    enum_iter(iter_type value) : m_pos{value} {}

    /**
     * @brief Pre-increment operator.
     *
     * @return A reference to the incremented iterator.
     */
    auto operator++() -> enum_iter & {
      ++m_pos;
      return *this;
    }

    /**
     * @brief Post-increment operator.
     *
     * @return A copy of the iterator before incrementing.
     */
    auto operator++(int) -> enum_iter {
      m_pos++;
      return *this;
    }

    /**
     * @brief Inequality comparison operator.
     *
     * @param other The other iterator to compare with.
     * @return True if the iterators are not equal, otherwise false.
     */
    auto operator!=(const enum_iter &other) const -> bool {
      return m_pos != other.m_pos;
    }

    /**
     * @brief Equality comparison operator.
     *
     * @param other The other iterator to compare with.
     * @return True if the iterators are equal, otherwise false.
     */
    auto operator==(const enum_iter &other) const -> bool {
      return m_pos == other.m_pos;
    }

    /**
     * @brief Dereference operator.
     *
     * @return The current enum pair.
     */
    auto operator*() const -> value_type;

  private:
    iter_type m_pos; /**< The current position of the iterator. */
  };

public:
  /**
   * @brief Default constructor.
   */
  enum_for_each() = default;

  /**
   * @brief Returns an iterator to the beginning of the enum range.
   *
   * @return A reference to the beginning iterator.
   */
  auto begin() -> enum_iter & { return m_begin; }

  /**
   * @brief Returns an iterator to the end of the enum range.
   *
   * @return A reference to the end iterator.
   */
  auto end() -> enum_iter & { return m_end; }

  /**
   * @brief Returns the size of the enum range.
   *
   * @return The size of the enum range.
   */
  auto size() -> std::size_t {
    return enum_range<Enum>::max - enum_range<Enum>::min;
  }

private:
  enum_iter m_begin{enum_range<Enum>::min}; /**< The beginning iterator. */
  enum_iter m_end{enum_range<Enum>::max};   /**< The end iterator. */
};
} // namespace mgutility

#endif // DETAIL_ENUM_FOR_EACH_HPP
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

#ifndef DETAIL_OPTIONAL_HPP
#define DETAIL_OPTIONAL_HPP

#include "mgutility/_common/definitions.hpp"
#include "mgutility/reflection/detail/meta.hpp"

#include <exception>

#if MGUTILITY_CPLUSPLUS >= 201703L
#include <optional>
#endif

namespace mgutility {

#if MGUTILITY_CPLUSPLUS < 201703L

/**
 * @brief Exception thrown when accessing an empty optional.
 */
struct bad_optional_access : public std::exception {
  /**
   * @brief Returns the exception message.
   *
   * @return A C-string with the exception message.
   */
  const char *what() const noexcept { return "optional has no value"; }
};

struct nullopt_t;

/**
 * @brief A class template that provides optional (nullable) objects.
 *
 * @tparam T The type of the value.
 */
template <typename T> class optional {
public:
  /**
   * @brief Constructs an empty optional.
   */
  MGUTILITY_CNSTXPR inline optional(nullopt_t &)
      : dummy_{0}, has_value_{false} {}

  /**
   * @brief Constructs an empty optional.
   */
  MGUTILITY_CNSTXPR inline optional() : dummy_{0}, has_value_{false} {}

  /**
   * @brief Constructs an optional with a value.
   *
   * @tparam Args The types of the arguments.
   * @param args The arguments to construct the value.
   */
  template <typename... Args>
  MGUTILITY_CNSTXPR inline optional(Args &&...args)
      : value_{T{std::forward<Args>(args)...}}, has_value_{true} {}

  /**
   * @brief Constructs an optional with a value.
   *
   * @param value The value to initialize with.
   */
  MGUTILITY_CNSTXPR inline optional(T &&value)
      : value_{value}, has_value_{true} {}

  /**
   * @brief Copy constructor.
   *
   * @param other The other optional to copy.
   */
  MGUTILITY_CNSTXPR inline optional(const optional &other)
      : value_{other.value_}, has_value_{other.has_value_} {}

  /**
   * @brief Move constructor.
   *
   * @param other The other optional to move.
   */
  MGUTILITY_CNSTXPR inline optional(optional &&other)
      : value_{other.value_}, has_value_{other.has_value_} {
    other.reset();
  }

  /**
   * @brief Destructor.
   */
  inline ~optional() { has_value_ = false; }

  /**
   * @brief Copy assignment operator.
   *
   * @param other The other optional to copy.
   * @return A reference to this optional.
   */
  MGUTILITY_CNSTXPR inline optional &operator=(const optional &other) {
    value_ = other.value_;
    has_value_ = other.has_value_;
    return *this;
  }

  /**
   * @brief Move assignment operator.
   *
   * @param other The other optional to move.
   * @return A reference to this optional.
   */
  MGUTILITY_CNSTXPR inline optional &operator=(optional &&other) {
    value_ = other.value_;
    has_value_ = other.has_value_;
    other.reset();
    return *this;
  }

  /**
   * @brief Swaps the contents of this optional with another.
   *
   * @param other The other optional to swap with.
   */
  MGUTILITY_CNSTXPR inline void swap(optional &&other) {
    auto val = std::move(other.value_);
    other.value_ = std::move(value_);
    value_ = std::move(val);

    auto hval = std::move(other.has_value_);
    other.has_value_ = std::move(has_value_);
    has_value_ = std::move(hval);
  }

  /**
   * @brief Dereferences the stored value.
   *
   * @return A reference to the stored value.
   */
  MGUTILITY_CNSTXPR inline T &operator*() { return value_; }

  /**
   * @brief Dereferences the stored value (const version).
   *
   * @return A reference to the stored value.
   */
  MGUTILITY_CNSTXPR inline T &operator*() const { return value_; }

  /**
   * @brief Accesses the stored value.
   *
   * @return A reference to the stored value.
   * @throws bad_optional_access if the optional has no value.
   */
  MGUTILITY_CNSTXPR inline T &value() {
    if (!has_value_)
      throw bad_optional_access();
    return value_;
  }

  /**
   * @brief Accesses the stored value (const version).
   *
   * @return A reference to the stored value.
   * @throws bad_optional_access if the optional has no value.
   */
  MGUTILITY_CNSTXPR inline T &value() const {
    if (!has_value_)
      throw bad_optional_access();
    return value_;
  }

  /**
   * @brief Returns the stored value or a default value if empty.
   *
   * @param value The default value to return if empty.
   * @return The stored value or the default value.
   */
  MGUTILITY_CNSTXPR inline T value_or(T &&value) {
    return has_value_ ? value_ : value;
  }

  /**
   * @brief Returns the stored value or a default value if empty (const
   * version).
   *
   * @param value The default value to return if empty.
   * @return The stored value or the default value.
   */
  MGUTILITY_CNSTXPR inline T value_or(T &&value) const {
    return has_value_ ? value_ : value;
  }

  /**
   * @brief Returns the stored value or a default value if empty.
   *
   * @param value The default value to return if empty.
   * @return The stored value or the default value.
   */
  MGUTILITY_CNSTXPR inline T value_or(const T &value) {
    return has_value_ ? value_ : value;
  }

  /**
   * @brief Returns the stored value or a default value if empty (const
   * version).
   *
   * @param value The default value to return if empty.
   * @return The stored value or the default value.
   */
  MGUTILITY_CNSTXPR inline T value_or(const T &value) const {
    return has_value_ ? value_ : value;
  }

  /**
   * @brief Constructs the value in-place.
   *
   * @param value The value to emplace.
   */
  MGUTILITY_CNSTXPR inline void emplace(T value) {
    value_ = std::move(value);
    has_value_ = true;
  }

  /**
   * @brief Constructs the value in-place with arguments.
   *
   * @tparam Args The types of the arguments.
   * @param args The arguments to construct the value.
   */
  template <typename... Args>
  MGUTILITY_CNSTXPR inline void emplace(Args &&...args) {
    value_ = std::move(T{std::forward<Args>(args)...});
    has_value_ = true;
  }

  /**
   * @brief Checks if the optional has a value.
   *
   * @return True if the optional has a value, otherwise false.
   */
  MGUTILITY_CNSTXPR inline bool has_value() const { return has_value_; }

  /**
   * @brief Resets the optional, making it empty.
   */
  template <typename U = T,
            detail::enable_if_t<!std::is_destructible<U>::value, bool> = true>
  MGUTILITY_CNSTXPR inline void reset() {
    value_ = T{};
    has_value_ = false;
  }

  /**
   * @brief Resets the optional, making it empty.
   */
  template <typename U = T,
            detail::enable_if_t<std::is_destructible<T>::value, bool> = true>
  MGUTILITY_CNSTXPR inline void reset() {
    value_.~T();
    has_value_ = false;
  }

  /**
   * @brief Checks if the optional has a value.
   *
   * @return True if the optional has a value, otherwise false.
   */
  MGUTILITY_CNSTXPR operator bool() { return has_value_; }

private:
  union {
    T value_;
    char dummy_[sizeof(T)];
  };
  bool has_value_;
};

/**
 * @brief Represents a null optional.
 */
struct nullopt_t {
  /**
   * @brief Converts nullopt_t to an empty optional.
   *
   * @tparam T The type of the optional.
   * @return An empty optional.
   */
  template <typename T> MGUTILITY_CNSTXPR operator optional<T>() {
    return optional<T>{};
  }
};

/**
 * @brief A global instance of nullopt_t to represent null optional.
 */
auto nullopt = nullopt_t{};

#else
/**
 * @brief Alias template for std::optional.
 *
 * @tparam T The type of the value.
 */
template <typename T> using optional = std::optional<T>;

/**
 * @brief A global constant representing a null optional.
 */
inline constexpr auto nullopt{std::nullopt};

#endif
} // namespace mgutility

#endif // DETAIL_OPTIONAL_HPP
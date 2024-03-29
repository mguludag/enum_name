/*
 * MIT License
 *
 * Copyright (c) 2023 Muhammed Galib Uludag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MGUTILITY_ENUM_NAME_HPP
#define MGUTILITY_ENUM_NAME_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(_MSC_VER) && _MSC_VER < 1910
#error "Requires MSVC 2017 or newer!"
#elif defined(__clang__) && __clang_major__ < 6
#error "Requires clang 6 or newer!"
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 9
#error "Requires gcc 9 or newer!"
#elif !defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
#error "Your compiler is not supported!"
#endif

#ifdef _MSC_VER
#define MG_ENUM_NAME_CPLUSPLUS _MSVC_LANG
#else
#define MG_ENUM_NAME_CPLUSPLUS __cplusplus
#endif

#if MG_ENUM_NAME_CPLUSPLUS == 201103L
#define MG_ENUM_NAME_CNSTXPR
#elif MG_ENUM_NAME_CPLUSPLUS > 201103L
#define MG_ENUM_NAME_CNSTXPR constexpr
#elif MG_ENUM_NAME_CPLUSPLUS < 201103L
#error "Standards older than C++11 is not supported!"
#endif

#if MG_ENUM_NAME_CPLUSPLUS > 201702L
#include <optional>
#endif

namespace mgutility {
namespace detail {

template <typename E>
struct is_scoped_enum {
    static constexpr auto value =
        std::is_enum<E>::value &&
        !std::is_convertible<E, typename std::underlying_type<E>::type>::value;
};
#if MG_ENUM_NAME_CPLUSPLUS > 201103L
template <typename E>
static constexpr bool is_scoped_enum_v = is_scoped_enum<E>::value;
#endif

template <typename T>
using underlying_type_t = typename std::underlying_type<T>::type;

template <typename T>
using remove_const_t = typename std::remove_const<T>::type;

#if MG_ENUM_NAME_CPLUSPLUS < 201703L

template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

constexpr auto strlen_constexpr(const char* str,
                                size_t sz = 0) noexcept -> size_t {
    return str[sz] == '\0' ? sz : strlen_constexpr(str, ++sz);
}

template <typename Char = char>
class basic_string_view {
   public:
    constexpr inline basic_string_view() noexcept : data_(""), size_(0) {}
    constexpr inline basic_string_view(const Char* str) noexcept
        : data_(str), size_(strlen_constexpr(str)) {}
    constexpr inline basic_string_view(const Char* str, size_t len) noexcept
        : data_(str), size_(len) {}
    constexpr inline basic_string_view(const basic_string_view& other)
        : data_(other.data_), size_(other.size_) {}
    constexpr inline basic_string_view(basic_string_view&& other) noexcept
        : data_(std::move(other.data_)), size_(std::move(other.size_)) {}
    MG_ENUM_NAME_CNSTXPR inline basic_string_view<Char>& operator=(
        const basic_string_view& other) noexcept {
        data_ = other.data_;
        size_ = other.size_;
        return *this;
    }
    MG_ENUM_NAME_CNSTXPR inline basic_string_view<Char>& operator=(
        basic_string_view&& other) noexcept {
        data_ = std::move(other.data_);
        size_ = std::move(other.size_);
        return *this;
    }
    constexpr inline const Char operator[](size_t index) const noexcept {
        return data_[index];
    }
    constexpr inline const Char* begin() const noexcept { return data_; }
    constexpr inline const Char* end() const noexcept {
        return (data_ + size_);
    }
    constexpr inline bool empty() const noexcept { return size_ < 1; }
    constexpr inline size_t size() const noexcept { return size_; }
    constexpr inline const Char* data() const noexcept { return data_; }
    constexpr inline basic_string_view<Char> substr(size_t begin,
                                                    size_t len) const noexcept {
        return basic_string_view<Char>(data_ + begin, len);
    }
    constexpr inline size_t rfind(Char c, size_t pos = 0) const noexcept {
        return c == data_[pos] ? pos : rfind(c, --pos);
    }

    constexpr friend inline bool operator==(
        basic_string_view<Char> lhs, basic_string_view<Char> rhs) noexcept {
        return (lhs.size_ == rhs.size_) &&
               std::strncmp(lhs.data_, rhs.data_, lhs.size_) == 0;
    }

    constexpr friend inline bool operator==(basic_string_view<Char> lhs,
                                            const Char* rhs) noexcept {
        return (lhs.size_ == strlen_constexpr(rhs)) &&
               std::strncmp(lhs.data_, rhs, lhs.size_) == 0;
    }

    constexpr friend inline bool operator!=(
        basic_string_view<Char> lhs, basic_string_view<Char> rhs) noexcept {
        return !(lhs == rhs);
    }

    constexpr friend inline bool operator!=(basic_string_view<Char> lhs,
                                            const Char* rhs) noexcept {
        return !(lhs == rhs);
    }

    inline operator std::string() { return std::string(data_, size_); }
    inline operator std::string() const { return std::string(data_, size_); }

    friend inline std::ostream& operator<<(std::ostream& os,
                                           const basic_string_view<Char>& sv) {
        for (auto c : sv) {
            os << c;
        }
        return os;
    }

   private:
    size_t size_;
    const Char* data_;
};

using string_view = basic_string_view<char>;

struct bad_optional_access : public std::exception {
    const char* what() const noexcept { return "optional has no value"; }
};

struct nullopt_t;

template <typename T>
class optional {
   public:
    MG_ENUM_NAME_CNSTXPR inline optional(nullopt_t&)
        : dummy_{0}, has_value_{false} {}
    MG_ENUM_NAME_CNSTXPR inline optional() : dummy_{0}, has_value_{false} {}
    template <typename... Args>
    MG_ENUM_NAME_CNSTXPR inline optional(Args&&... args)
        : value_{T{std::forward<Args>(args)...}}, has_value_{true} {}
    MG_ENUM_NAME_CNSTXPR inline optional(T&& value)
        : value_{value}, has_value_{true} {}
    MG_ENUM_NAME_CNSTXPR inline optional(const optional& other)
        : value_{other.value_}, has_value_{other.has_value_} {}
    MG_ENUM_NAME_CNSTXPR inline optional(optional&& other)
        : value_{other.value_}, has_value_{other.has_value_} {
        other.reset();
    }
    inline ~optional() { has_value_ = false; }
    MG_ENUM_NAME_CNSTXPR inline optional& operator=(const optional& other) {
        value_ = other.value_;
        has_value_ = other.has_value_;
        return *this;
    }
    MG_ENUM_NAME_CNSTXPR inline optional& operator=(optional&& other) {
        value_ = other.value_;
        has_value_ = other.has_value_;
        other.reset();
        return *this;
    }
    MG_ENUM_NAME_CNSTXPR inline void swap(optional&& other) {
        auto val = std::move(other.value_);
        other.value_ = std::move(value_);
        value_ = std::move(val);

        auto hval = std::move(other.has_value_);
        other.has_value_ = std::move(has_value_);
        has_value_ = std::move(hval);
    }
    MG_ENUM_NAME_CNSTXPR inline T& operator*() { return value_; }
    MG_ENUM_NAME_CNSTXPR inline T& operator*() const { return value_; }
    MG_ENUM_NAME_CNSTXPR inline T& value() {
        if (!has_value_) throw detail::bad_optional_access();
        return value_;
    }
    MG_ENUM_NAME_CNSTXPR inline T& value() const {
        if (!has_value_) throw detail::bad_optional_access();
        return value_;
    }
    MG_ENUM_NAME_CNSTXPR inline T value_or(T&& value) {
        return has_value_ ? value_ : value;
    }
    MG_ENUM_NAME_CNSTXPR inline T value_or(T&& value) const {
        return has_value_ ? value_ : value;
    }
    MG_ENUM_NAME_CNSTXPR inline T value_or(const T& value) {
        return has_value_ ? value_ : value;
    }
    MG_ENUM_NAME_CNSTXPR inline T value_or(const T& value) const {
        return has_value_ ? value_ : value;
    }
    MG_ENUM_NAME_CNSTXPR inline void emplace(T value) {
        value_ = std::move(value);
        has_value_ = true;
    }
    template <typename... Args>
    MG_ENUM_NAME_CNSTXPR inline void emplace(Args&&... args) {
        value_ = std::move(T{std::forward<Args>(args)...});
        has_value_ = true;
    }
    MG_ENUM_NAME_CNSTXPR inline bool has_value() const { return has_value_; }

#if !(defined(__clang__) && __clang_major__ < 11)
    template <typename U = T,
              enable_if_t<!std::is_destructible<U>::value, bool> = true>
    MG_ENUM_NAME_CNSTXPR inline void reset() {
        T::~T();
        has_value_ = false;
    }
#endif
    template <typename U = T,
              enable_if_t<std::is_destructible<T>::value, bool> = true>
    MG_ENUM_NAME_CNSTXPR inline void reset() {
        value_ = T{};
        has_value_ = false;
    }

    MG_ENUM_NAME_CNSTXPR operator bool() { return has_value_; }

   private:
    union {
        T value_;
        char dummy_[sizeof(T)];
    };
    bool has_value_;
};

struct nullopt_t {
    template <typename T>
    MG_ENUM_NAME_CNSTXPR operator optional<T>() {
        return optional<T>{};
    }
};

auto nullopt = nullopt_t{};

#else

template <typename T>
using optional = std::optional<T>;
inline constexpr auto nullopt{std::nullopt};
using string_view = std::string_view;
template <bool B, class T = void>
using enable_if_t = std::enable_if_t<B, T>;

#endif

template <typename Enum, Enum...>
struct enum_sequence {};

template <typename Enum, int Min, int Max, int... Next>
struct enum_sequence_helper
    : enum_sequence_helper<Enum, Min, Max - 1, Max - 1, Next...> {};

template <typename Enum, int Min, int... Next>
struct enum_sequence_helper<Enum, Min, Min, Next...> {
    using type = enum_sequence<Enum, static_cast<Enum>(Next)...>;
};

template <typename Enum, int Min, int Max>
using make_enum_sequence = typename enum_sequence_helper<Enum, Min, Max>::type;

struct enum_type {
#if defined(_MSC_VER)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
    template <
        typename Enum, Enum e,
        detail::enable_if_t<!detail::is_scoped_enum<Enum>::value, bool> = true>
    MG_ENUM_NAME_CNSTXPR static inline auto name() noexcept
        -> detail::string_view {
        auto str = detail::string_view(__PRETTY_FUNCTION__);
        auto offset{lastidxenumname[0] + lastidxenumname[1]};
        auto index =
            std::max(str.rfind(lastidxenumname[2], str.size() - offset),
                     str.rfind(lastidxenumname[3], str.size() - offset));
        auto result = str.substr(index + 1, str.size() - offset - index);
        return result[0] == '(' ? "" : result;
    }

    template <
        typename Enum, Enum e,
        detail::enable_if_t<detail::is_scoped_enum<Enum>::value, bool> = true>
    MG_ENUM_NAME_CNSTXPR static inline auto name() noexcept
        -> detail::string_view {
        auto str = detail::string_view(__PRETTY_FUNCTION__);
        auto index =
            str.rfind(lastidxenumname[3], str.size() - lastidxenumname[0]) + 1;
        auto result =
            str.substr(index, str.size() - lastidxenumname[0] - index);
        return result.size() > 4 ? result[4] == lastidxenumname[4] ? "" : result
                                 : result;
    }

   private:
    static constexpr int lastidxenumname[] =
#if defined(_MSC_VER)
        {22, 0, ',', ':', '<'};
#elif defined(__clang__)
        {1, 1, ' ', ':', '('};
#elif defined(__GNUC__)
        {
#if MG_ENUM_NAME_CPLUSPLUS < 201703L
            179,
#else
            165,
#endif
            5, ' ', ':', '('};
#endif
};

template <typename Enum>
using enum_pair = std::pair<Enum, detail::string_view>;

template <typename Enum, Enum... Is>
MG_ENUM_NAME_CNSTXPR inline auto get_enum_array(
    detail::enum_sequence<Enum, Is...>) noexcept
    -> std::array<detail::string_view, sizeof...(Is) + 1> {
    static std::array<detail::string_view,
                                           sizeof...(Is) + 1>
        arr{"", enum_type::template name<Enum, Is>()...};
    return arr;
}

template <typename Enum, int Min, int Max>
inline auto to_enum_impl(detail::string_view str) noexcept
    -> detail::optional<Enum> {
    auto arr = get_enum_array<Enum>(detail::make_enum_sequence<Enum, Min, Max>());
    const auto index{std::find(arr.begin() + 1, arr.end(), str)};
    return index == arr.end()
               ? detail::nullopt
               : detail::optional<Enum>{static_cast<Enum>(
                     std::distance(arr.begin(), index) + Min - 1)};
}

template <typename Enum, int Min, int Max>
inline auto enum_name_impl(Enum e) noexcept -> detail::string_view {
    auto arr = get_enum_array<Enum>(detail::make_enum_sequence<Enum, Min, Max>());
    const auto index{std::abs(Min) + static_cast<int>(e) + (Min < 0 ? 1 : 1)};
    return arr[(index < Min || index > arr.size() - 1) ? 0 : index];
}
}  // namespace detail
}  // namespace mgutility

namespace mgutility {
template <typename T>
struct enum_range {
    static constexpr auto min{-128};
    static constexpr auto max{128};
};

template <typename Enum>
class enum_for_each {
    using value_type = const detail::enum_pair<Enum>;
    using size_type = std::size_t;

    struct enum_iter {
        using const_iter_type = decltype(enum_range<Enum>::min);
        using iter_type = detail::remove_const_t<const_iter_type>;
        using iterator_category = std::forward_iterator_tag;
        using value_type = const detail::enum_pair<Enum>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        enum_iter() : m_pos{} {}
        enum_iter(iter_type value) : m_pos{value} {}

        auto operator++() -> enum_iter& {
            ++m_pos;
            return *this;
        }

        auto operator++(int) -> enum_iter {
            m_pos++;
            return *this;
        }

        auto operator!=(const enum_iter& other) const -> bool {
            return m_pos != other.m_pos;
        }

        auto operator==(const enum_iter& other) const -> bool {
            return m_pos == other.m_pos;
        }

        auto operator*() const -> value_type;

       private:
        iter_type m_pos;
    };

   public:
    enum_for_each() {}
    auto begin() -> enum_iter& { return m_begin; }
    auto end() -> enum_iter& { return m_end; }
    auto size() -> std::size_t {
        return enum_range<Enum>::max - enum_range<Enum>::min;
    }

   private:
    enum_iter m_begin{enum_range<Enum>::min};
    enum_iter m_end{enum_range<Enum>::max};
};

template <typename Enum>
constexpr inline auto enum_to_underlying(Enum e) noexcept
    -> detail::underlying_type_t<Enum> {
    static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
    return static_cast<detail::underlying_type_t<Enum>>(e);
}

template <int Min, int Max, typename Enum>
MG_ENUM_NAME_CNSTXPR inline auto enum_name(Enum e) noexcept
    -> detail::string_view {
    static_assert(Min < Max - 1, "Max must be greater than (Min + 1)!");
    static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
    return detail::enum_name_impl<Enum, Min, Max>(e);
}

template <typename Enum, int Min = enum_range<Enum>::min,
          int Max = enum_range<Enum>::max>
MG_ENUM_NAME_CNSTXPR inline auto enum_name(Enum e) noexcept
    -> detail::string_view {
    static_assert(Min < Max - 1, "Max must be greater than (Min + 1)!");
    static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
    return detail::enum_name_impl<Enum, Min, Max>(e);
}

template <typename Enum>
auto enum_for_each<Enum>::enum_iter::operator*() const -> value_type {
    auto value = static_cast<Enum>(m_pos);
    return detail::enum_pair<Enum>{value, enum_name(value)};
}

template <typename Enum, int Min = enum_range<Enum>::min,
          int Max = enum_range<Enum>::max>
MG_ENUM_NAME_CNSTXPR inline auto to_enum(detail::string_view str) noexcept
    -> detail::optional<Enum> {
    static_assert(Min < Max - 1, "Max must be greater than (Min + 1)!");
    static_assert(std::is_enum<Enum>::value, "Type is not an Enum type!");
    return detail::to_enum_impl<Enum, Min, Max>(str);
}
}  // namespace mgutility

template <typename Enum,
          typename std::enable_if<std::is_enum<Enum>::value, bool>::type = true>
auto operator<<(std::ostream& os, Enum e) -> std::ostream& {
    static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
    os << mgutility::enum_name(e);
    return os;
}

#endif  // MGUTILITY_ENUM_NAME_HPP

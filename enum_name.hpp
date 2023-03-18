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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
#include <cstring>
#include <string>
#include <type_traits>

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
#define __CPLUSPLUS _MSVC_LANG
#else
#define __CPLUSPLUS __cplusplus
#endif

#if __CPLUSPLUS == 201103L
#define CNSTXPR
#elif __CPLUSPLUS > 201103L
#define CNSTXPR constexpr
#elif __CPLUSPLUS < 201103L
#error "Standards older than C++11 is not supported!"
#endif

#if __CPLUSPLUS > 201702L
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

template <typename Char = char>
class basic_string_view {
   public:
    constexpr inline basic_string_view(const Char* str = nullptr) noexcept
        : str_(str), size_(strlen(str)) {}
    constexpr inline basic_string_view(const Char* str, size_t len) noexcept
        : str_(str), size_(len) {}
    constexpr inline basic_string_view(const basic_string_view& other) noexcept
        : str_(other.str_), size_(other.size_) {}
    CNSTXPR inline basic_string_view<Char>& operator=(
        const basic_string_view& other) noexcept {
        str_ = other.str_;
        size_ = other.size_;
        return *this;
    }
    CNSTXPR inline const Char operator[](size_t index) noexcept { return str_[index]; }
    CNSTXPR inline const Char* begin() noexcept { return str_; }
    CNSTXPR inline const Char* end() noexcept { return (str_ + size_); }
    constexpr inline size_t size() const noexcept { return size_; }
    constexpr inline const Char* data() const noexcept { return str_; }
    CNSTXPR inline basic_string_view<Char> substr(size_t begin,
                                                  size_t len) noexcept {
        return basic_string_view<Char>(str_ + begin, len);
    }
    CNSTXPR inline basic_string_view<Char> substr(size_t begin,
                                                  size_t len) const noexcept {
        return basic_string_view<Char>(str_ + begin, len);
    }
    CNSTXPR inline size_t rfind(Char c, size_t offset = 0) const noexcept {
        for (auto i{size_ - offset - 1}; i != 0; --i) {
            if (c == str_[i]) return i;
        }
        return 0;
    }

    CNSTXPR friend inline bool operator==(basic_string_view<Char> lhs, basic_string_view<Char> rhs) noexcept {
        if(lhs.size_ != rhs.size_)
            return false;
        for(auto i{0}; i < lhs.size_; ++i){
            if(lhs.str_[i] != rhs.str_[i])
                return false;
        }
        return true;
    }

    friend inline std::ostream& operator<<(std::ostream& os,
                                           const basic_string_view<Char>& sv) {
        for (auto i = 0; i < sv.size_; ++i) {
            os << sv.str_[i];
        }
        return os;
    }

   private:
    const Char* str_;
    size_t size_;
};

using string_view = basic_string_view<char>;

template <typename Char, size_t N>
class basic_static_string {
   public:
    CNSTXPR inline basic_static_string(const Char* str = nullptr) noexcept
        : size_(strlen(str)) {
        data_[size_] = npos;
        std::copy_n(str, size_, &data_[0]);
    }
    CNSTXPR inline basic_static_string(const Char* str, size_t len) noexcept
        : size_{len} {
        data_[size_] = npos;
        std::copy_n(str, size_, &data_[0]);
    }
    CNSTXPR inline basic_static_string(
        const basic_static_string& other) noexcept
        : size_(other.size_) {
        data_[size_] = npos;
        std::copy_n(&other.data_[0], size_, &data_[0]);
    }
    CNSTXPR inline basic_static_string(basic_static_string&& other) noexcept
        : size_(std::move(other.size_)) {
        data_[size_] = npos;
        std::move(&other.data_[0], &other.data_[0] + size_, &data_[0]);
    }
    CNSTXPR inline basic_static_string<Char, N>& operator=(
        const basic_static_string& other) noexcept {
        data_ = other.data_;
        size_ = other.size_;
        return *this;
    }
    CNSTXPR inline basic_static_string<Char, N>& operator=(
        basic_static_string&& other) noexcept {
        data_ = std::move(other.data_);
        size_ = std::move(other.size_);
        return *this;
    }
    CNSTXPR inline Char* begin() const noexcept { return data_; }
    CNSTXPR inline Char* end() const noexcept { return (data_ + size_); }

    CNSTXPR inline size_t size() const noexcept { return size_; }
    CNSTXPR inline const Char* data() const noexcept { return data_; }
    CNSTXPR inline const Char* c_str() const noexcept { return data_; }
    CNSTXPR inline basic_static_string<Char, N> substr(
        size_t len, size_t begin = 0) noexcept {
        return basic_static_string<Char, N>(&data_[begin], len);
    }
    CNSTXPR inline basic_static_string<Char, N> substr(
        size_t len, size_t begin = 0) const noexcept {
        return basic_static_string<Char, N>(&data_[begin], len);
    }

    inline operator std::string() { return std::string(&data_[0], size_); }

    friend inline std::ostream& operator<<(
        std::ostream& os, const basic_static_string<Char, N>& sv) {
        for (auto i = 0; i < sv.size_; ++i) {
            os << sv.data_[i];
        }
        return os;
    }

   private:
    static constexpr Char npos{'\0'};
    size_t size_;
    Char data_[N];
};

template <size_t N = 128>
using static_string = basic_static_string<char, N>;

#if __CPLUSPLUS < 201703L

template <typename T, typename... Args>
class optional {
   public:
    CNSTXPR inline optional(Args&&... args)
        : value_{T{std::forward<Args>(args)...}}, has_value_{true} {}
    CNSTXPR inline optional(T&& value) : value_{value}, has_value_{true} {}
    CNSTXPR inline optional(std::nullptr_t) : dummy_{0}, has_value_{false} {}
    CNSTXPR inline optional(const optional& other)
        : value_{other.value_}, has_value_{other.has_value_} {}
    CNSTXPR inline optional(optional&& other)
        : value_{other.value_}, has_value_{other.has_value_} {
        other.reset();
    }
    inline ~optional() { has_value_ = false; }
    CNSTXPR inline optional& operator=(const optional& other) {
        value_ = other.value_;
        has_value_ = other.has_value_;
        return *this;
    }
    CNSTXPR inline optional& operator=(optional&& other) {
        value_ = other.value_;
        has_value_ = other.has_value_;
        other.reset();
        return *this;
    }
    CNSTXPR inline void swap(optional&& other) {
        auto val = std::move(other.value_);
        other.value_ = std::move(value_);
        value_ = std::move(val);

        auto hval = std::move(other.has_value_);
        other.has_value_ = std::move(has_value_);
        has_value_ = std::move(hval);
    }
    CNSTXPR inline T& operator*() { return value_; }
    CNSTXPR inline T& operator*() const { return value_; }
    CNSTXPR inline T& value() { return value_; }
    CNSTXPR inline T& value() const { return value_; }
    CNSTXPR inline T& value_or(T&& value) {
        return has_value_ ? value_ : value;
    }
    CNSTXPR inline T& value_or(T&& value) const {
        return has_value_ ? value_ : value;
    }
    CNSTXPR inline void emplace(T value) {
        value_ = std::move(value);
        has_value_ = true;
    }
    CNSTXPR inline void emplace(Args&&... args) {
        value_ = std::move(T{std::forward<Args>(args)...});
        has_value_ = true;
    }
    CNSTXPR inline bool has_value() const { return has_value_; }

#if !(defined(__clang__) && __clang_major__ < 11)
    template <typename U = T,
              typename std::enable_if<!std::is_destructible<U>::value,
                                      bool>::type = true>
    CNSTXPR inline void reset() {
        T::~T();
        has_value_ = false;
    }
#endif
    template <typename U = T,
              typename std::enable_if<std::is_destructible<T>::value,
                                      bool>::type = true>
    CNSTXPR inline void reset() {
        value_ = T{};
        has_value_ = false;
    }

    CNSTXPR operator bool() { return has_value_; }

   private:
    union {
        T value_;
        char dummy_[sizeof(T)];
    };
    bool has_value_;
};

struct nullopt_t {
    template <typename T>
    CNSTXPR operator optional<T>() {
        return optional<T>{nullptr};
    }
};

auto nullopt = nullopt_t{};

#else

template <typename T>
using optional = std::optional<T>;
inline constexpr auto nullopt{std::nullopt};

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
    template <typename Enum, Enum e,
              typename std::enable_if<!detail::is_scoped_enum<Enum>::value,
                                      bool>::type = true>
    static inline auto name() noexcept -> detail::string_view {
        auto str = detail::string_view(__PRETTY_FUNCTION__);
        auto offset{lastidxenumname[0] + lastidxenumname[1]};
        auto index = std::max(str.rfind(lastidxenumname[2], offset), str.rfind(lastidxenumname[3], offset)) + 1;
        return str.substr(index, str.size() - offset - index);
    }

    template <typename Enum, Enum e,
              typename std::enable_if<detail::is_scoped_enum<Enum>::value,
                                      bool>::type = true>
    static inline auto name() noexcept -> detail::string_view {
        auto str = detail::string_view(__PRETTY_FUNCTION__);
        auto index = str.rfind(lastidxenumname[3], lastidxenumname[0]) + 1;
        auto result = str.substr(index, str.size() - lastidxenumname[0] - index);
        return result.size() > 4 ? result[4] == lastidxenumname[4] ? "" : result : result;
    }

   private:
    static constexpr int lastidxenumname[] =
#if defined(_MSC_VER)
        {21, 0, ',', ':', '<'};
#elif defined(__clang__)
        {1, 0, ' ', ':', '('};
#elif defined(__GNUC__)
        {179, 4, ' ', ':', '('};
#endif
};

template <typename Enum, Enum... Is>
CNSTXPR inline auto __for_each_to_enum_impl(
    detail::string_view str, int Min,
    detail::enum_sequence<Enum, Is...>) noexcept -> detail::optional<Enum> {
    using expander = detail::string_view[];
    const expander x{"", enum_type::template name<Enum, Is>()...};

    for (auto i{1}; i < sizeof...(Is); ++i) {
        if (x[i] == str)
            return detail::optional<Enum>{static_cast<Enum>(i + Min - 1)};
    }

    return detail::nullopt;
}

template <typename Enum, Enum... Is>
CNSTXPR inline auto __for_each_enum_impl(
    Enum e, int Min, int Max, detail::enum_sequence<Enum, Is...>) noexcept
    -> detail::string_view {
    using expander = detail::string_view[];
    const expander x{"", enum_type::template name<Enum, Is>()...};
    auto index{abs(Min) + static_cast<int>(e) + 1};
    return detail::string_view(x[index > Max + 1 ? 0 : index]);
}
}  // namespace detail
}  // namespace mgutility

namespace mgutility {
template <int Min = 0, int Max = 256, typename Enum>
CNSTXPR inline auto enum_name(Enum e) noexcept -> detail::static_string<256> {
    static_assert(Min < Max - 1, "Max must be greater than (Min + 1)!");
    static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
    auto str = __for_each_enum_impl(
        e, Min, Max, mgutility::detail::make_enum_sequence<Enum, Min, Max>());
    return {str.data(), str.size()};
}

template <typename Enum, int Min = 0, int Max = 256>
CNSTXPR inline auto to_enum(detail::string_view str) noexcept
    -> detail::optional<Enum> {
    static_assert(Min < Max - 1, "Max must be greater than (Min + 1)!");
    static_assert(std::is_enum<Enum>::value, "Type is not an Enum type!");
    return __for_each_to_enum_impl(
        str, Min, detail::make_enum_sequence<Enum, Min, Max>());
}
}  // namespace mgutility

#endif  // MGUTILITY_ENUM_NAME_HPP

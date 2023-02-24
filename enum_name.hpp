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


namespace mgutility{
namespace detail{
template <typename Char = char>
class basic_string_view
{
public:
    constexpr inline basic_string_view(const Char* str = nullptr) noexcept : str_(str), size_(strlen(str)) {}
    constexpr inline basic_string_view(const Char* str, size_t len) noexcept : str_(str), size_(len) {}
    constexpr inline basic_string_view(const basic_string_view& other) noexcept : str_(other.str_), size_(other.size_) {}
    CNSTXPR inline basic_string_view<Char>& operator=(const basic_string_view& other) noexcept {
        str_ = other.str_;
        size_ = other.size_;
        return *this;
    }
    constexpr inline size_t size() const noexcept { return size_; }
    constexpr inline const Char* data() const noexcept { return str_; }
    CNSTXPR inline basic_string_view<Char> substr(size_t len, size_t begin = 0) noexcept { 
        return basic_string_view<Char>(str_ + begin, len); 
    }
    CNSTXPR inline basic_string_view<Char> substr(size_t len, size_t begin = 0) const noexcept { 
        return basic_string_view<Char>(str_ + begin, len); 
    }

    friend inline std::ostream& operator<<(std::ostream& os, const basic_string_view<Char>& sv){
        for(auto i = 0; i < sv.size_; ++i){
            os << sv.str_[i];
        }
        return os;
    }

private:
    const Char* str_;
    size_t size_;
};

using string_view = basic_string_view<char>;
    
#if __CPLUSPLUS > 201103L
template <typename Char, size_t N>
class basic_static_string
{
public:
    constexpr inline basic_static_string(const Char* str = nullptr) noexcept : size_(strlen(str)) {
        data_[size_] = '\0';
        std::copy_n(str, size_, &data_[0]);
    }
    constexpr inline basic_static_string(const Char* str, size_t len) noexcept : size_(len) {
        data_[size_] = '\0';
        std::copy_n(str, size_, &data_[0]);
    }
    constexpr inline basic_static_string(const basic_static_string& other) noexcept : size_(other.size_) {
        data_[size_] = '\0';
        std::copy_n(&other.data_[0], size_, &data_[0]);
    }
    constexpr inline basic_static_string(basic_static_string&& other) noexcept : size_(std::move(other.size_)) {
        data_[size_] = '\0';
        std::move(&other.data_[0], &other.data_[0] + size_, &data_[0]);
    }
    constexpr inline basic_static_string<Char, N>& operator=(const basic_static_string& other) noexcept {
        data_ = other.data_;
        size_ = other.size_;
        return *this;
    }
    constexpr inline basic_static_string<Char, N>& operator=(basic_static_string&& other) noexcept {
        data_ = std::move(other.data_);
        size_ = std::move(other.size_);
        return *this;
    }
    constexpr inline size_t size() const noexcept { return size_; }
    constexpr inline const Char* data() const noexcept { return data_; }
    constexpr inline const Char* c_str() const noexcept { return data_; }
    constexpr inline basic_static_string<Char, N> substr(size_t len, size_t begin = 0) noexcept { 
        return basic_static_string<Char, N>(&data_[begin], len); 
    }
    constexpr inline basic_static_string<Char, N> substr(size_t len, size_t begin = 0) const noexcept { 
        return basic_static_string<Char, N>(&data_[begin], len);
    }

    inline operator std::string(){
        return std::string(&data_[0], size_);
    }

    friend inline std::ostream& operator<<(std::ostream& os, const basic_static_string<Char, N>& sv){
        for(auto i = 0; i < sv.size_; ++i){
            os << sv.data_[i];
        }
        return os;
    }

private:
    size_t size_;
    Char data_[N]; 
};

template <size_t N = 128>
using static_string = basic_static_string<char, N>;
#endif

template <typename Enum, Enum ...>
struct enum_sequence{};

template <typename Enum, int Min, int Max, int ... Next>
struct enum_sequence_helper : enum_sequence_helper<Enum, Min, Max-1, Max-1, Next...>{};

template <typename Enum, int Min, int ... Next>
struct enum_sequence_helper<Enum, Min, Min, Next ... >{ using type = enum_sequence<Enum, static_cast<Enum>(Next) ... >; };

template <typename Enum, int Min, int Max>
using make_enum_sequence = typename enum_sequence_helper<Enum, Min, Max>::type;

struct enum_type
{
#if defined(_MSC_VER)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
    template <typename Enum, Enum e>
    static inline auto name(Enum) noexcept -> detail::string_view {
        const auto s = enum_type::name<Enum>().size();
        const auto str = detail::string_view(__PRETTY_FUNCTION__);
        return str.substr(str.size() - idxenumval[0] - idxenumval[1] - idxenumval[2] - (s * idxenumval[3]), idxenumval[0] + idxenumval[1] + s); 
    }

    template <typename Enum>
    CNSTXPR static inline auto name() noexcept -> detail::string_view { 
        const auto str = detail::string_view(__PRETTY_FUNCTION__);
        return str.substr(str.size() - idxenumname[0] - idxenumname[1], idxenumname[0]); 
    }

private:
    static constexpr int idxenumname[] // {begin, size-end}
#if defined(_MSC_VER)
    {97, 7};
#elif defined(__clang__)
    {71, 1};
#elif defined(__GNUC__)
    {88, 78};
#endif

    static constexpr int idxenumval[]
#if defined(_MSC_VER)
    {88, 1, 8, 2};
#elif defined(__clang__)
    {75, 6, 1, 1};
#elif defined(__GNUC__)
    {92, 11, 78, 1};
#endif
};

template <typename Enum, Enum... Is>
CNSTXPR inline auto __for_each_enum_impl(Enum e, int Min, detail::enum_sequence<Enum, Is...>) noexcept -> detail::string_view {
    using expander = detail::string_view[];
    const expander x{"", enum_type::template name<Enum, Is>(e)...};
    return detail::string_view(x[abs(Min) + static_cast<int>(e) + 1]);
}
} // namespace detail
} // namespace mgutility

namespace mgutility{
template <int Min = 0, int Max = 256, typename Enum>
#if __CPLUSPLUS == 201103L
inline auto enum_name(Enum e) -> std::string {
    static_assert(Min < Max - 1, "Max must be greater than (Min + 1)!");
    static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
    const auto str = __for_each_enum_impl(e, Min, mgutility::detail::make_enum_sequence<Enum, Min, Max>());
    return std::string(str.data(), str.size());
}
#elif __CPLUSPLUS > 201103L
constexpr inline auto enum_name(Enum e) noexcept -> detail::static_string<128> {
    static_assert(Min < Max - 1, "Max must be greater than (Min + 1)!");
    static_assert(std::is_enum<Enum>::value, "Value is not an Enum type!");
    auto str = __for_each_enum_impl(e, Min, mgutility::detail::make_enum_sequence<Enum, Min, Max>());
    return detail::static_string<128>(str.data(), str.size());
}
#endif
} // namespace mgutility

#endif // MGUTILITY_ENUM_NAME_HPP

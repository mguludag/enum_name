#ifndef MGUTILITY_ENUM_NAME_HPP
#define MGUTILITY_ENUM_NAME_HPP

#include <cstring>
#include <string>

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
#endif


namespace mgutility{
namespace detail{
template <typename Char = char>
class basic_string_view
{
public:
    constexpr basic_string_view(const Char* str = nullptr) : str_(str), size_(strlen(str)) {}
    constexpr basic_string_view(const Char* str, size_t len) : str_(str), size_(len) {}
    constexpr basic_string_view(const basic_string_view& other) : str_(other.str_), size_(other.size_) {}
    CNSTXPR basic_string_view<Char>& operator=(const basic_string_view& other) {
        str_ = other.str_;
        size_ = other.size_;
        return *this;
    }
    constexpr size_t size() const { return size_; }
    constexpr const Char* data() const { return str_; }
    CNSTXPR basic_string_view<Char> substr(size_t len, size_t begin = 0) { 
        return basic_string_view<Char>(str_ + begin, len); 
    }

    friend std::ostream& operator<<(std::ostream& os, const basic_string_view<Char>& sv){
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
    CNSTXPR static inline auto name(Enum v) -> detail::string_view {
        const auto s = enum_type::name<Enum>().size();
        auto str = detail::string_view(__PRETTY_FUNCTION__);
        return str.substr(str.size() - idxenumval[0] - idxenumval[1] - idxenumval[2] - (s * idxenumval[3]), idxenumval[0] + idxenumval[1] + s); 
    }

    template <typename Enum>
    CNSTXPR static inline auto name() -> detail::string_view { 
        auto str = detail::string_view(__PRETTY_FUNCTION__);
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
    {97, 1, 8, 2};
#elif defined(__clang__)
    {75, 6, 1, 1};
#elif defined(__GNUC__)
    {92, 11, 78, 1};
#endif
};

template <typename Enum, Enum... Is>
CNSTXPR inline auto __for_each_enum_impl(Enum e, int Min, detail::enum_sequence<Enum, Is...>) -> detail::string_view {
    using expander = detail::string_view[];
    const expander x{"", enum_type::template name<Enum, Is>(e)...};
    const auto str = detail::string_view(x[abs(Min) + static_cast<int>(e) + 1]);
    return detail::string_view(str.data(), str.size());
}
} // namespace detail
} // namespace mgutility

namespace mgutility{
template <int Min = 0, int Max = 256, typename Enum>
inline auto enum_name(Enum e) -> std::string {
    static_assert(Min < Max - 1, "Max must be higher than (Min + 1)!");
    auto str = __for_each_enum_impl(e, Min, mgutility::detail::make_enum_sequence<Enum, Min, Max>());
    return std::string(str.data(), str.size());
}
} // namespace mgutility

#endif // MGUTILITY_ENUM_NAME_HPP

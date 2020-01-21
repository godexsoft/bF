#pragma once

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace util
{
template <typename T> static int constexpr hex_width()
{
    return sizeof(T) * 2;
}

template <typename T> inline static std::string hex(T value)
{
    constexpr int width = hex_width<T>();

    std::stringstream iss;
    iss << std::setfill('0') << std::right << std::setw(width) << std::hex << static_cast<int64_t>(value);

    // the above code sometimes wraps and returns values like "ffffffffffff8a" because
    // we are casting it to the highest possible int (64bit) to get the final hex value.
    // to print only the relevant part we just cut off everything that is not in our interest:
    std::string out = iss.str();
    return out.substr(out.length() - width, width);
}

template <typename T> static char to_readable(T value, char placeholder = '.')
{
    if (std::isalnum(value))
    {
        return value;
    }

    return placeholder;
}

template <typename T> static std::string bold_underline(const T &data)
{
    return std::string("\u001b[1m\u001b[4m") + data + "\u001b[0m";
}
} // namespace util
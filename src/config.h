#pragma once

namespace bf
{
#ifdef ENABLE_8BIT
inline constexpr bool Enable8 = true;
#else
inline constexpr bool Enable8 = false;
#endif

#ifdef ENABLE_16BIT
inline constexpr bool Enable16 = true;
#else
inline constexpr bool Enable16 = false;
#endif

#ifdef ENABLE_32BIT
inline constexpr bool Enable32 = true;
#else
inline constexpr bool Enable32 = false;
#endif

#ifdef ENABLE_64BIT
inline constexpr bool Enable64 = true;
#else
inline constexpr bool Enable64 = false;
#endif
} // namespace bf
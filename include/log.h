#pragma once

#include <fmt/format.h>

namespace bf
{
#ifdef ENABLE_LOG
inline constexpr bool EnableLog = true;
#else
inline constexpr bool EnableLog = false;
#endif

class logger
{
  public:
    static logger &instance()
    {
        static logger log;
        return log;
    }

    logger(bool enable = false)
        : enable_(enable)
    {
    }

    void enable(bool enable) { enable_ = enable; }
    bool enabled() const { return enable_; }

    template <typename... Args> void info(const char *format, const Args &... args)
    {
        if (EnableLog)
        {
            if (enabled())
            {
                print("[INFO]", format, fmt::make_format_args(args...));
            }
        }
    }

    template <typename... Args> void fatal(const char *format, const Args &... args)
    {
        print("[FATAL]", format, fmt::make_format_args(args...));
    }

  private:
    void print(const char *prefix, const char *format, fmt::format_args args)
    {
        fmt::print("{} ", prefix);
        fmt::vprint(format, args);
        fmt::print("\n");
    }

    bool enable_;
};
} // namespace bf
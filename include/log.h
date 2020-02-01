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
  private:
    logger()
        : enable_(false)
    {
    }

  public:
    logger(const logger &) = delete;
    logger &operator=(const logger &) = delete;
    logger(logger &&) = delete;
    logger &operator=(logger &&) = delete;

    static auto &instance()
    {
        static logger log;
        return log;
    }

    void enable(bool enable) { enable_ = enable; }
    bool enabled() const { return enable_; }

    template <typename... Args> void info(const char *format, const Args &... args)
    {
        if constexpr (EnableLog)
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
        fmt::print(stderr, "{} ", prefix);
        fmt::vprint(stderr, format, args);
        fmt::print(stderr, "\n");
    }

    bool enable_;
};
} // namespace bf
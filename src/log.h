#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdarg.h>

#define MAX_LOG 2048

namespace bf
{
#ifdef ENABLE_LOG
static constexpr bool EnableLog = true;
#else
static constexpr bool EnableLog = false;
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

    void print(const std::string &fmt, ...)
    {
        if (enable_)
        {
            va_list ap;
            char buffer[MAX_LOG];

            va_start(ap, fmt.c_str());
            vsnprintf(buffer, MAX_LOG, fmt.c_str(), ap);
            std::cerr << buffer << std::endl;
            va_end(ap);
        }
    }

    void fatal(const std::string &fmt, ...)
    {
        va_list ap;
        char buffer[MAX_LOG];

        va_start(ap, fmt.c_str());
        vsnprintf(buffer, MAX_LOG, fmt.c_str(), ap);
        std::cerr << "[FATAL] " << buffer << std::endl;
        va_end(ap);
    }

  private:
    bool enable_;
};
} // namespace bf
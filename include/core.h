#pragma once

#include "memory.h"
#include "parser.h"

#include <fstream>
#include <iostream>
#include <map>

#include <fmt/format.h>

namespace bf
{
template <typename T> class core
{
  public:
    using memoryt = memory<T>;
    using parsert = parser<T>;

    core(std::string_view file, uint64_t cells, uint64_t start_cell, bool elastic, bool wrapping)
        : memory_{cells, start_cell, elastic, wrapping}
        , parser_{file}
        , targets_(cells, 0)
    {
        tape_.reserve(1024); // probably enough for most programs. will grow if needed
    }

    int execute()
    {
        compile();
        run();

        return 0; // for now assume all good
    }

  private:
    void compile()
    {
        std::vector<uint64_t> loops;
        loops.reserve(128); // should be enough depth for most programs

        // parsing
        parser_.parse([&](auto cursor, auto act) {
            tape_.push_back(act);

            switch (act)
            {
            case action::loop_start:
                loops.push_back(cursor);
                break;
            case action::loop_end: {
                if (loops.empty())
                {
                    logger::instance().fatal("unmatched ']' at {}", cursor);
                    exit(-127);
                }

                auto idx = loops.back();
                targets_[cursor] = idx;
                targets_[idx] = cursor;
                loops.pop_back();
            }
            break;
            default:
                // not interested at these here
                break;
            }
        });

        if (!loops.empty())
        {
            logger::instance().fatal("unmatched '[' at {}", loops.back());
            exit(-127);
        }
    }

    void run()
    {
        for (auto cursor = 0; cursor < tape_.size(); ++cursor)
        {
            switch (tape_.at(cursor))
            {
            case action::loop_start:
                if (memory_.is_zero())
                {
                    cursor = targets_.at(cursor);
                }
                break;
            case action::loop_end:
                if (!memory_.is_zero())
                {
                    cursor = targets_.at(cursor);
                }
                break;
            case action::increment:
                memory_.inc();
                break;
            case action::decrement:
                memory_.dec();
                break;
            case action::move_left:
                memory_.left();
                break;
            case action::move_right:
                memory_.right();
                break;
            case action::output: {
                // only print \n if it's a 10 (bf way)
                auto c = memory_.read();
                if (c == T(10))
                {
                    fmt::print("\n");
                }
                else
                {
                    fmt::print("{}", c);
                }
            }
            break;
            case action::input: {
                char c{};

                std::cin.clear();
                std::cin.get(c);

                if (std::cin.fail())
                {
                    logger::instance().info("EOF received");
                    ++cursor; // skip input

                    continue;
                }

                if (c == '\n' || c == '\r')
                {
                    memory_.write(T{10}); // 10 is bf way to write \n
                }
                else
                {
                    memory_.write(c);
                }
            }
            break;
            case action::memory_dump:
                memory_.dump();
                break;
            default:
                // nop
                break;
            }
        }
    }

    memoryt memory_;
    parsert parser_;

    std::vector<uint64_t> targets_;
    std::vector<action> tape_;
};
} // namespace bf

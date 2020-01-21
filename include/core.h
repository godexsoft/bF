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

    core(std::string_view file, int cells, int start_cell, bool elastic, bool wrapping)
        : memory_{cells, start_cell, elastic, wrapping}
        , parser_{file}
        , targets_(cells, 0)
    {
    }

    void run()
    {
        std::vector<int> loops;

        loops.reserve(128);     // should be enough depth for most programs
        actions_.reserve(1024); // probably enough for most programs. will grow if needed

        // parsing
        parser_.parse([&](auto cursor, auto act) {
            actions_.push_back(act);

            switch (act)
            {
            case loop_start:
                loops.push_back(cursor);
                break;
            case loop_end: {
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

        // runtime
        for (auto cursor = 0; cursor < actions_.size(); ++cursor)
        {
            auto act = actions_.at(cursor);

            if (act == loop_start)
            {
                if (memory_.is_zero())
                {
                    cursor = targets_.at(cursor);
                }
            }
            else if (act == loop_end)
            {
                if (!memory_.is_zero())
                {
                    cursor = targets_.at(cursor);
                }
            }
            else if (act == increment)
            {
                memory_.inc();
            }
            else if (act == decrement)
            {
                memory_.dec();
            }
            else if (act == move_left)
            {
                memory_.left();
            }
            else if (act == move_right)
            {
                memory_.right();
            }
            else if (act == output)
            {
                // only print \n if it's a 10 (bf way)
                char c = memory_.read();
                if (c == T(10))
                {
                    fmt::print("\n");
                }
                else
                {
                    fmt::print("{}", c);
                }
            }
            else if (act == input)
            {
                char c;

                std::cin.clear();
                std::cin.get(c);

                if (std::cin.fail())
                {
                    logger::instance().info("EOF received");
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
            else if (act == memory_dump)
            {
                memory_.dump();
            }
            else if (act == start_of_input)
            {
                // nop at runtime
            }
        }
    }

  private:
    memoryt memory_;
    parsert parser_;

    std::vector<int> targets_;
    std::vector<action> actions_;
};
} // namespace bf

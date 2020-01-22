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
        tape_.reserve(1024); // probably enough for most programs. will grow if needed
    }

    void execute()
    {
        compile();
        run();
    }

  private:
    void compile()
    {
        std::vector<int> loops;
        loops.reserve(128); // should be enough depth for most programs

        // parsing
        parser_.parse([&](auto cursor, auto act) {
            tape_.push_back(act);

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
    }

    void run()
    {
        for (auto cursor = 0; cursor < tape_.size(); ++cursor)
        {
            switch (tape_.at(cursor))
            {
            case loop_start:
                if (memory_.is_zero())
                {
                    cursor = targets_.at(cursor);
                }
                break;
            case loop_end:
                if (!memory_.is_zero())
                {
                    cursor = targets_.at(cursor);
                }
                break;
            case increment:
                memory_.inc();
                break;
            case decrement:
                memory_.dec();
                break;
            case move_left:
                memory_.left();
                break;
            case move_right:
                memory_.right();
                break;
            case output: {
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
            case input: {
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
            case memory_dump:
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

    std::vector<unsigned int> targets_;
    std::vector<action> tape_;
};
} // namespace bf

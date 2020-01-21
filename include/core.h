#pragma once

#include "memory.h"
#include "parser.h"

#include <fstream>
#include <iostream>
#include <map>

namespace bf
{
template <typename T> class core
{
  public:
    using memoryt = memory<T>;
    using parsert = parser<T>;
    using sizet = typename memory<T>::sizet;

    core(std::string_view file, sizet cells, sizet start_cell, bool elastic, bool wrapping)
        : memory_(cells, start_cell, elastic, wrapping)
        , parser_(file)
        , targets_(cells, 0)
    {
    }

    int run()
    {
        action in;
        std::vector<sizet> loops;
        sizet parsing_cursor = 0;

        loops.reserve(128);     // should be enough depth for most programs
        actions_.reserve(1024); // probably enough for most programs. will grow if needed

        // parsing
        while ((in = parser_.next()) != action::eof)
        {
            actions_.push_back(in);

            if (in == loop_start)
            {
                loops.push_back(parsing_cursor);
            }
            else if (in == loop_end)
            {
                if (loops.empty())
                {
                    logger::instance().fatal("unmatched ']' at %d", parsing_cursor);
                    exit(-127);
                }

                auto idx = loops.back();
                targets_[parsing_cursor] = idx;
                targets_[idx] = parsing_cursor;
                loops.pop_back();
            }

            ++parsing_cursor;
        }

        if (!loops.empty())
        {
            logger::instance().fatal("unmatched '[' at %d", loops.back());
            exit(-127);
        }

        logger::instance().info("RUNTIME");

        // runtime
        for (auto cursor = 0; cursor < actions_.size(); ++cursor)
        {
            action act = actions_.at(cursor);

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
                    std::cout << std::endl;
                }
                else
                {
                    std::cout << c;
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
                    ++cursor; // skip this input
                    continue;
                }

                if (c == '\n' || c == '\r')
                {
                    memory_.write(T(10)); // 10 is bf way to write \n
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

        return -1;
    }

  private:
    memoryt memory_;
    parsert parser_;

    std::vector<sizet> targets_;
    std::vector<action> actions_;
};
} // namespace bf

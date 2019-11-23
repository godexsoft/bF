#pragma once

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

#include "log.h"
#include "util.h"

namespace bf
{
template <typename T> class memory
{
  public:
    typedef T cellt;
    typedef typename std::vector<cellt>::size_type sizet;

    memory(sizet cells, sizet start_cell = 0, bool elastic = true, bool wrapping = true)
        : cell_idx_(start_cell)
        , elastic_(elastic)
        , wrapping_(wrapping)
    {
        allocate(cells);
    }

    inline void inc()
    {
        ++model_[cell_idx_];

        if constexpr (bf::EnableLog)
        {
            if (logger::instance().enabled())
            {
                logger::instance().print("+ [%d] = %d (0x%s)", cell_idx_, static_cast<int>(model_[cell_idx_]),
                                         util::hex(static_cast<int>(model_[cell_idx_])).c_str());
            }
        }
    }

    inline void dec()
    {
        --model_[cell_idx_];

        if constexpr (bf::EnableLog)
        {
            if (logger::instance().enabled())
            {
                logger::instance().print("- [%d] = %d (0x%s)", cell_idx_, static_cast<int>(model_[cell_idx_]),
                                         util::hex(static_cast<int>(model_[cell_idx_])).c_str());
            }
        }
    }

    inline void right()
    {
        if (++cell_idx_ >= capacity_)
        {
            if (elastic_)
            {
                capacity_ *= 2;
                model_.resize(capacity_);
            }
            else
            {
                logger::instance().fatal("out of bounds.");
                exit(-127);
            }
        }

        if constexpr (bf::EnableLog)
        {
            if (logger::instance().enabled())
            {
                sizet orig_cell = cell_idx_;
                logger::instance().print("> [%d]=>[%d]", orig_cell, cell_idx_);
            }
        }
    }

    inline void left()
    {
        // wrap around if needed
        if (--cell_idx_ < 0)
        {
            if (wrapping_)
            {
                cell_idx_ = capacity_ - 1;
            }
            else
            {
                logger::instance().fatal("negative out of bounds.");
                exit(-127);
            }
        }

        if constexpr (bf::EnableLog)
        {
            if (logger::instance().enabled())
            {
                sizet orig_cell = cell_idx_;
                logger::instance().print("< [%d]=>[%d]", orig_cell, cell_idx_);
            }
        }
    }

    inline bool is_zero() { return model_[cell_idx_] == 0; }

    inline char read()
    {
        if constexpr (bf::EnableLog)
        {
            if (logger::instance().enabled())
            {
                logger::instance().print(". [%d] = %d (0x%s)", cell_idx_, static_cast<int>(model_[cell_idx_]),
                                         util::hex(static_cast<int>(model_[cell_idx_])).c_str());
            }
        }
        return static_cast<char>(model_[cell_idx_]);
    }

    inline void write(char value)
    {
        model_[cell_idx_] = T(value);

        if constexpr (bf::EnableLog)
        {
            if (logger::instance().enabled())
            {
                logger::instance().print(", [%d] = %d (0x%s)", cell_idx_, static_cast<int>(model_[cell_idx_]),
                                         util::hex(static_cast<int>(model_[cell_idx_])).c_str());
            }
        }
    }

    void dump()
    {
        auto distance = 128;                                                  // total cells to show in dump
        auto from_cell = std::max(0, static_cast<int>(cell_idx_ - distance)); // find cell to start from
        auto to_cell = from_cell + distance * 2;                              // find cell to dump till

        // adjust amount of cells shown per row depending on how wide the hex will be
        auto cells_per_row = 8;
        if constexpr (std::is_same<T, int8_t>())
        {
            cells_per_row = 16;
        }
        else if constexpr (std::is_same<T, int64_t>())
        {
            cells_per_row = 4;
        }

        auto separator_at = cells_per_row / 2 - 1; // place extra separator in the middle

        std::cerr << std::endl; // make some space on the screen

        std::stringstream dump_ss;
        std::stringstream content_ss;
        auto column = 0;

        for (auto cell = from_cell; cell < to_cell; ++cell)
        {
            // add content as character
            char ch = util::to_readable(model_[cell]);
            if (cell == cell_idx_)
            {
                content_ss << util::bold_underline(ch);
            }
            else
            {
                content_ss << ch;
            }

            // prepend with starting cell id in this row
            if (column == 0)
            {
                dump_ss << std::setfill('0') << std::right << std::setw(8) << cell << " ";
            }

            dump_ss << " ";
            if (cell == cell_idx_)
            {
                dump_ss << util::bold_underline(util::hex(model_[cell]));
            }
            else
            {
                dump_ss << util::hex(model_[cell]);
            }

            if (column == separator_at)
            {
                dump_ss << " ";
            }

            if (column >= cells_per_row - 1)
            {
                column = 0;
                dump_ss << "  |" << content_ss.str() << "|" << std::endl;

                // cleanup the current content line stream
                content_ss.str(std::string());
                content_ss.clear();

                continue;
            }

            ++column;
        }

        // dump the last bit
        dump_ss << std::endl;
        std::cerr << dump_ss.str();
    }

  private:
    inline void allocate(sizet cells)
    {
        capacity_ = cells;
        model_.resize(capacity_, 0);
    }

    sizet cell_idx_; // current cell ptr
    sizet capacity_; // initial capacity. this is used to wrap around

    bool elastic_;  // if we wanna allocate infinite space dynamically
    bool wrapping_; // if we wanna wrap around on negative

    std::vector<cellt> model_;
};

} // namespace bf

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
    using cell_t = T;

    memory(uint64_t cells, uint64_t start_cell = 0, bool elastic = true, bool wrapping = true)
        : cell_idx_{start_cell}
        , capacity_{cells}
        , elastic_{elastic}
        , wrapping_{wrapping}
        , model_{}
    {
        allocate(cells);
    }

    void inc() noexcept
    {
        ++model_[cell_idx_];
        debug_log('+');
    }

    void dec() noexcept
    {
        --model_[cell_idx_];
        debug_log('-');
    }

    void right()
    {
        auto orig_cell{cell_idx_};

        if (++cell_idx_ >= capacity_)
        {
            if (elastic_)
            {
                allocate(capacity_ * 2);
            }
            else
            {
                logger::instance().fatal("overflow");
                throw std::runtime_error("overflow");
            }
        }

        debug_log('>', orig_cell);
    }

    void left()
    {
        auto orig_cell{cell_idx_};

        // wrap around if needed
        if (--cell_idx_ < 0)
        {
            if (wrapping_)
            {
                cell_idx_ = capacity_ - 1;
            }
            else
            {
                logger::instance().fatal("underflow");
                throw std::runtime_error("underflow");
            }
        }

        debug_log('<', orig_cell);
    }

    bool is_zero() const noexcept { return model_[cell_idx_] == 0; }

    char read() const noexcept
    {
        debug_log('.');
        return static_cast<char>(model_[cell_idx_]);
    }

    void write(char value) noexcept
    {
        model_[cell_idx_] = T(value);
        debug_log(',');
    }

    void dump() const
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

        std::cout << std::endl; // make some space on the screen

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
        std::cout << dump_ss.str();
    }

  private:
    void debug_log(char op) const
    {
        if (EnableLog)
        {
            logger::instance().info("{} [{}] = {} (0x{})", op, cell_idx_, static_cast<int>(model_[cell_idx_]),
                                    util::hex(static_cast<int>(model_[cell_idx_])).c_str());
        }
    }

    void debug_log(char op, int idx) const
    {
        if (EnableLog)
        {
            logger::instance().info("{} [{}]=>[{}]", op, idx, cell_idx_);
        }
    }

    void allocate(int cells)
    {
        capacity_ = cells;
        model_.resize(capacity_, T{});
    }

    uint64_t cell_idx_; // current cell ptr
    uint64_t capacity_; // initial capacity. this is used to wrap around

    bool elastic_;  // if we wanna allocate infinite space dynamically
    bool wrapping_; // if we wanna wrap around on negative

    std::vector<cell_t> model_;
}; // class memory
} // namespace bf

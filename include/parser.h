#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>

namespace bf
{
//      Spec:
// >	Move the pointer to the right
// <	Move the pointer to the left
// +	Increment the memory cell under the pointer
// -	Decrement the memory cell under the pointer
// .	Output the character signified by the cell at the pointer
// ,	Input a character and store it in the cell at the pointer
// [	Jump past the matching ] if the cell under the pointer is 0
// ]	Jump back to the matching [ if the cell under the pointer is nonzero
//
//      Extensions:
// !    If input file is not specified this symbol will separate code from input data in STDIN
// #    Prints a full memory dump around current memory pointer

enum class action : char
{
    eof = 0,
    move_left = '<',
    move_right = '>',
    increment = '+',
    decrement = '-',
    output = '.',
    input = ',',
    loop_start = '[',
    loop_end = ']',
    memory_dump = '#',
    start_of_input = '!',
    invalid = -1
};

template <typename T> class parser
{
  public:
    parser(std::string_view file)
    {
        if (!file.empty())
        {
            file_stream_ = std::make_unique<std::ifstream>(file.data());
            if (!*file_stream_)
            {
                logger::instance().fatal("input file '{}' could not be read.", file.data());
                std::exit(-1);
            }
        }
    }

    int parse(std::function<int(unsigned int, action)> cb)
    {
        auto cursor{0};
        auto act{action::invalid};

        do
        {
            act = parse_action(next());
            if (!file_stream_ && act == action::start_of_input)
            {
                return cb(cursor, action::eof);
            }

            if (act != action::invalid)
            {
                auto err = cb(cursor, act);
                if (err != 0)
                {
                    return err;
                }
                ++cursor;
            }
        } while (act != action::eof);

        return 0;
    }

  private:
    action parse_action(std::optional<char> c) const
    {
        if (!c)
        {
            return action::eof;
        }

        switch (*c)
        {
        case '.':
            return action::output;
        case ',':
            return action::input;
        case '[':
            return action::loop_start;
        case ']':
            return action::loop_end;
        case '<':
            return action::move_left;
        case '>':
            return action::move_right;
        case '+':
            return action::increment;
        case '-':
            return action::decrement;
        case '!':
            return action::start_of_input;
        case '#':
            return action::memory_dump;
        default:
            return action::invalid;
        }
    }

    std::optional<char> next()
    {
        char c;

        if (file_stream_)
        {
            if (!file_stream_->get(c))
            {
                file_stream_->close();
                return std::nullopt;
            }
        }
        else
        {
            std::cin.get(c);
            if (std::cin.fail())
            {
                return std::nullopt;
            }
        }

        return c;
    }

    std::unique_ptr<std::ifstream> file_stream_;
}; // namespace bf
} // namespace bf
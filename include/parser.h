#pragma once

#include <fstream>
#include <iostream>
#include <memory>

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

enum action : char
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
                logger::instance().fatal("input file '%s' could not be read.", file.data());
                std::exit(-1);
            }
        }
    }

    action next()
    {
        char c;
        do
        {
            if (file_stream_)
            {
                if (!file_stream_->get(c))
                {
                    file_stream_->close();
                    return eof;
                }
            }
            else
            {
                std::cin.get(c);
                if (std::cin.fail())
                {
                    return eof;
                }
            }

            if (is_valid(c))
            {
                auto act = action_for_char(c);
                if (!file_stream_ && act == start_of_input)
                {
                    return eof;
                }

                return act;
            }
        } while (!is_valid(c));

        return eof;
    }

    inline bool is_valid(char c) const
    {
        if (action_for_char(c) != invalid)
        {
            return true;
        }

        return false;
    }

    inline action action_for_char(char c) const
    {
        switch (c)
        {
        case '.':
            return output;
        case ',':
            return input;
        case '[':
            return loop_start;
        case ']':
            return loop_end;
        case '<':
            return move_left;
        case '>':
            return move_right;
        case '+':
            return increment;
        case '-':
            return decrement;
        case '!':
            return start_of_input;
        case '#':
            return memory_dump;
        default:
            return invalid;
        }
    }

  private:
    std::unique_ptr<std::ifstream> file_stream_;
}; // namespace bf
} // namespace bf
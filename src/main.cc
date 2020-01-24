#include "config.h"
#include "core.h"
#include "cxx_argp_parser.h"
#include "log.h"
#include "util.h"

#include <fstream>
#include <iostream>

using namespace std;
using namespace bf;

bool constexpr var_cellsize_enabled()
{
    auto enabled{0};

    if (bf::Enable8)
    {
        ++enabled;
    }

    if (bf::Enable16)
    {
        ++enabled;
    }

    if (bf::Enable32)
    {
        ++enabled;
    }

    if (bf::Enable64)
    {
        ++enabled;
    }

    return enabled > 1;
}

int main(int argc, char *argv[])
{
    cxx_argp::parser args{1}; // "-" for stdin input

    uint64_t stack_size{30000};
    uint8_t cell_size{8};
    uint64_t start_cell{0};
    auto elastic{false};
    auto wrapping{false};
    string file{};
    auto logging{false};

    args.add_option({"stack-size", 's', "cells", 0, "Stack size in cells"}, stack_size);
    args.add_option({"start-cell", 'i', "cell", 0, "Cell index for start cell"}, start_cell);

    if constexpr (var_cellsize_enabled())
    {
        args.add_option({"cell-size", 'c', "bits", 0, "Cell size in bits"}, cell_size);
    }

    args.add_option({"elastic", 'e', nullptr, 0, "Infinite array of cells"}, elastic);
    args.add_option({"wrapping", 'w', nullptr, 0, "Wrap to array size on negative index"}, wrapping);

    if constexpr (bf::EnableLog)
    {
        args.add_option({"logging", 'l', nullptr, 0, "Log to stderr"}, logging);
    }

    if (!args.parse(argc, argv, "[filename or -]", "Options:"))
    {
        logger::instance().fatal("there was an error parsing args");
        return 1;
    }

    // use file passed as argument or stdin
    auto f = args.arguments().at(0);
    if (f != "-")
    {
        file = f;
    }

    logger::instance().enable(logging);

    logger::instance().info("stack size {} cells", stack_size);
    logger::instance().info("start at cell: {}", start_cell);
    logger::instance().info("elastic memory: {}", elastic ? "yes" : "no");
    logger::instance().info("wrapping: {}", wrapping ? "yes" : "no");

    if constexpr (bf::Enable8)
    {
        if (cell_size <= 8)
        {
            logger::instance().info("cell size: 8 bit");
            return core<int8_t>{file, stack_size, start_cell, elastic, wrapping}.execute();
        }
    }

    if constexpr (bf::Enable16)
    {
        if (cell_size <= 16)
        {
            logger::instance().info("cell size: 16 bit");
            return core<int16_t>{file, stack_size, start_cell, elastic, wrapping}.execute();
        }
    }

    if constexpr (bf::Enable32)
    {
        if (cell_size <= 32)
        {
            logger::instance().info("cell size: 32 bit");
            return core<int32_t>{file, stack_size, start_cell, elastic, wrapping}.execute();
        }
    }

    if constexpr (bf::Enable64)
    {
        if (cell_size <= 64)
        {
            logger::instance().info("cell size: 64 bit");
            return core<int64_t>{file, stack_size, start_cell, elastic, wrapping}.execute();
        }
    }

    // no core found
    std::string supported{};
    if (bf::Enable8)
    {
        supported += "8 ";
    }
    if (bf::Enable16)
    {
        supported += "16 ";
    }
    if (bf::Enable32)
    {
        supported += "32 ";
    }
    if (bf::Enable64)
    {
        supported += "64 ";
    }

    if (!supported.empty())
    {
        supported.pop_back();
    }
    else
    {
        // realistically no idea why anyone would want to do this but it's supported.
        supported = "none";
    }

    logger::instance().fatal("invalid cell size. supported: {}", supported.substr());
    return -1;
}

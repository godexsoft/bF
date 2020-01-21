#include "core.h"
#include "cxx_argp_parser.h"
#include "log.h"
#include "util.h"

#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    cxx_argp::parser args{1}; // "-" for stdin input

    int64_t stack_size = 30000;
    int64_t cell_size = 8;
    int64_t start_cell = 0;
    bool elastic = false;
    bool wrapping = false;
    std::string file;
    bool logging = false;

    args.add_option({"stack-size", 's', "cells", 30000, "Stack size in cells"}, stack_size);
    args.add_option({"cell-size", 'c', "bits", 0, "Cell size in bits"}, cell_size);
    args.add_option({"start-cell", 'i', "cell", 0, "Cell index for start cell"}, start_cell);

    args.add_option({"elastic", 'e', nullptr, 0, "Infinite array of cells"}, elastic);
    args.add_option({"wrapping", 'w', nullptr, 0, "Wrap to array size on negative index"}, wrapping);

    if constexpr (bf::EnableLog)
    {
        args.add_option({"logging", 'l', nullptr, 0, "Log to stderr"}, logging);
    }

    if (!args.parse(argc, argv, "[filename or -]", "Options:"))
    {
        bf::logger::instance().fatal("there was an error parsing args");
        return 1;
    }

    // use file passed as argument or stdin
    auto f = args.arguments().at(0);
    if (f != "-")
    {
        file = f;
    }

    if constexpr (bf::EnableLog)
    {
        bf::logger::instance().enable(logging);

        bf::logger::instance().info("stack size {} cells", stack_size);
        bf::logger::instance().info("cell size: {} bytes (will round to closest high)", cell_size);
        bf::logger::instance().info("start at cell: {}", start_cell);
        bf::logger::instance().info("elastic memory: {}", elastic ? "yes" : "no");
        bf::logger::instance().info("wrapping: {}", wrapping ? "yes" : "no");
    }

    if (cell_size <= 8)
    {
        return bf::core<int8_t>(file, stack_size, start_cell, elastic, wrapping).run();
    }
    else if (cell_size <= 16)
    {
        return bf::core<int16_t>(file, stack_size, start_cell, elastic, wrapping).run();
    }
    else if (cell_size <= 32)
    {
        return bf::core<int32_t>(file, stack_size, start_cell, elastic, wrapping).run();
    }
    else if (cell_size <= 64)
    {
        return bf::core<int64_t>(file, stack_size, start_cell, elastic, wrapping).run();
    }
    else
    {
        bf::logger::instance().fatal("invalid cell size. supported values: 8, 16, 32 and 64.");
    }

    return -1;
}

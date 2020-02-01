#include "config.h"
#include "core.h"
#include "log.h"
#include "util.h"
#include <cxxopts.hpp>

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
    uint64_t stack_size{30000};
    uint8_t cell_size{8};
    uint64_t start_cell{0};
    auto elastic{false};
    auto wrapping{false};
    string file{};
    auto logging{false};

    try
    {
        cxxopts::Options options("bF", "Brainfuck interpreter written in C++17");

        // clang-format off
        options.add_options()
            ("s,stack-size", "Stack size in cells", cxxopts::value<uint64_t>(stack_size))        
            ("i,start-cell", "Cell index for start cell", cxxopts::value<uint64_t>(start_cell))
            ("e,elastic", "Infinite array of cells", cxxopts::value<bool>(elastic))
            ("w,wrapping", "Wrap on out of bounds", cxxopts::value<bool>(wrapping))
            ("input", "Input file (can also be specified as first argument)", cxxopts::value<std::string>(), "filename")        
            ("h,help", "Help message")
        ;
        // clang-format on

        if constexpr (var_cellsize_enabled())
        {
            options.add_options()("c,cell-size", "Cell size in bits", cxxopts::value<uint8_t>());
        }

        if constexpr (bf::EnableLog)
        {
            options.add_options()("l,logging", "Log to stderr", cxxopts::value<bool>(logging));
        }

        options.positional_help("[filename]").show_positional_help();
        options.parse_positional({"input"});

        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            fmt::print(options.help({""}));
            return 0;
        }

        if (result.count("input"))
        {
            file = result["input"].as<std::string>();
        }
    }
    catch (const cxxopts::OptionException &e)
    {
        logger::instance().fatal("Error parsing options: {}", e.what());
        return 1;
    }

    logger::instance().enable(logging);

    logger::instance().info("stack size: {} cells", stack_size);
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
        supported.pop_back(); // removes last space
    }
    else
    {
        // realistically no idea why anyone would want to do this but it's supported.
        supported = "none";
    }

    logger::instance().fatal("invalid cell size. supported: {}", supported.substr());
    return -1;
}

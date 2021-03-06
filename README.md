# bF - brainfuck interpreter

A more-or-less complete implementation of brainfuck in C++17.

## features
- running programs from files or stdin
- program code and input as one string (using ! to separate code from data)
- hex memory dump on demand (using # in bf code)
- wrapping
- fixed or elastic ("infinite") memory
- setting the starting point in memory
- optional logging on each step of execution (if compiled with logging support)

## clone with submodules

    $ git clone --recurse-submodules https://github.com/godexsoft/bF

## dependencies

- cxxopts (submodule, header-only)
- libfmt (submodule, header-only)

## configuring

The project is configured and built using CMake.

Logging is disabled by default but can be enabled using *BF_ENABLE_LOG* cmake option:

    $ cmake -DBF_ENABLE_LOG=ON ..

To configure and build bF:

    $ mkdir build && cd build && cmake ..
    $ make

By default this will build bF in *Release* configuration.

## running

    $ ./bF -h

## examples

There is an examples folder with a bunch of programs to play with. 
You can either specify the file as first argument or as *--input* option:

    $ ./bF ../examples/hello.bf
    $ ./bF --input ../examples/hello3.bf -i 10

or read input from stdin if no filename is specified

    $ ./bF < ../examples/hello.bf
    $ echo ',[.,]!Hello' | ./bF

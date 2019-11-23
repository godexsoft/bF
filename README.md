# bF - brainfuck interpreter

A more-or-less complete implementation of brainfuck in c++17.

## features
- running programs from files or stdin
- program code and input as one string (using ! to separate code from data)
- hex memory dump on demand (using # in bf code)
- wrapping
- fixed or elastic ("infinite") memory
- setting the starting point in memory
- optional logging on each step of execution

## clone with submodules

    $ git clone --recurse-submodules https://github.com/godexsoft/bF

## dependencies

- argp (if not present should be installed separately) 

using brew:
    
    $ brew install argp-standalone

## configuring

The project is configured and built using CMake.
In the CMakeLists.txt file at the top logging can be enabled or disabled. 

    set(BF_ENABLE_LOG true)

It's disabled by default.

To configure and build bF:

    $ mkdir build && cd build && cmake ..
    $ make

## running

    $ ./bF -?

## examples

There is an examples folder with a bunch of programs to play with. 
You can either specify the file as first argument

    $ ./bF ../examples/hello.bf

or read input from stdin if you pass "-" instead

    $ ./bF - < ../examples/hello.bf
    $ echo ",[.,]!Hello" | ./bF -

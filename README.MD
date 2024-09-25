# JSONc
> "The most creative name for a project."
> 
> \- no one ever

## What do we have here?

It is a JSON parser written in C(99). It has very minimal functionality right now. 

## Things that kind of work

1. Parsing JSON that include the following:
   - Strings
   - Numbers
   - Objects that only include Strings and Numbers.
2. Printing what you parsed.
3. The struct that is returned from the parser should be usable enough to write your own code around to get values out of them.

## TODO

- Arrays
  - Probably going to have to start with arrays of a single data type and work from there to support arrays where the values are all different types (legal JSON).
- Documentation 
  - You can look at `jsonc.h` to get an overview of the data types. Nothing is documented though. 

## Requirements

- CMAKE
- C compiler

## Building 

`cd <source_directory>; mkdir build; cd build; cmake .. ;cmake --build .;`

This will build the test application and jsonc as a library. You can literally rip out the files from `src` and  `include` and slap them in your project. Just make sure `jsonc.h` is in your include path, or change the `#include<jsonc.h>` in `jsonc.c` accordingly. 

## Contributing

One man show. Unless it isn't. Then it's not. I'll call you if I need you.

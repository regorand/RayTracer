#pragma once

#include <string>

// Takes advantage of the fact, that #include is a valid keyword in glsl to not get errors in IDE
static const std::string INCLUDE_PATTERN("#include");
static const size_t INCLUDE_PATTERN_LENGTH = INCLUDE_PATTERN.size();


// system needed more work -> The problems below ~should~ work now \\
    1. It can not deal with circular includes -> keep vector of already included files, ignore if file already in vector is included again \\
    2. The solution to the first problem will also fix the second one, multiple includes

// TODO Something that could be done to improve this: \\
    Keep a buffer of all Symbols defined and once recursive import is done, forward define all symbols, can reduce requirement of importing in the correct order

std::string loadShaderSource(std::string path, bool resolveCustomSymbols = true);
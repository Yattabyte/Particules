#pragma once
#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <numeric>

///////////////////////////////////////////////////////////////////////////
/// Useful definitions
constexpr unsigned int startingSeed = 0;

///////////////////////////////////////////////////////////////////////////
/// Credit to Sunsetquest on StackOverflow for original implementation
///     https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
inline static unsigned int fastRand() noexcept {
    static unsigned int gSeed = startingSeed;
    gSeed = (214013 * gSeed + 2531011);
    return (gSeed >> 16) & 0x7FFF;
}

#endif // UTILITIES_HPP
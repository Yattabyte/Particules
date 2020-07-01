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
    static thread_local unsigned int gSeed = startingSeed;
    gSeed = (214013 * gSeed + 2531011);
    return (gSeed >> 16) & 0x7FFF;
}

/** Holds mouse interaction information. */
struct MouseEvent {
    // Public Attributes
    double m_xPos = 0.0, m_yPos = 0.0;
    int m_mods = 0;

    // Public Enumerations
    /** The action states a key can be in: released, pressed, etc. */
    enum Action : unsigned int {
        RELEASE = 0b00000000,
        PRESS = 0b00000001,
        MOVE = 0b00000010,
    };
    unsigned int m_action = 0;
    /** The name of all keys supported, and an appropriate key code for each of
     * them. */
    enum class Key : int {
        LEFT = 0,
        RIGHT = 1,
        MIDDLE = 2,
    } m_button = Key::LEFT;
};

#endif // UTILITIES_HPP
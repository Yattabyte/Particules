#pragma once
#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include "Utility/vec.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// Application definitions
constexpr int WIDTH = 1024;
constexpr int HEIGHT = 768;
constexpr int CELL_SIZE = 64;
constexpr double TIME_STEP = 0.0125;
constexpr float ROOM_TEMP = 25.0F;
enum class MatterState { SOLID,
                         LIQUID,
                         GAS };
enum Attributes : unsigned int {
    INERT = 0b00000000,
    TURNS_TO_SOLID = 0b00000001,
    TURNS_TO_LIQUID = 0b00000010,
    TURNS_TO_GAS = 0b00000100,
    ON_FIRE = 0b00001000,
    FLAMMABLE = 0b00010000,
    EXPLOSIVE = 0b00110000,
    WET = 0b01000000,
};
enum class Element {
    AIR,
    SAND,
    SAWDUST,
    CONCRETE,
    FIRE,
    SMOKE,
    WATER,
    ICE,
    STEAM,
    OIL,
    GUNPOWDER,
    GASOLINE,
    METAL,
};
constexpr vec4 COLORS[] = {
    vec4(0),                          ///< AIR
    vec4(0.75F, 0.6F, 0.4F, 1.0F),    ///< SAND
    vec4(0.9F, 0.75F, 0.65F, 1.0F),   ///< SAWDUST
    vec4(0.4F, 0.4F, 0.4F, 1),        ///< CONCRETE
    vec4(1.0F, 0.2F, 0.1F, 1.0F),     ///< FIRE
    vec4(0.75F, 0.75F, 0.75F, 0.75F), ///< SMOKE
    vec4(0.1F, 0.2F, 1.0F, 1.0F),     ///< WATER
    vec4(0.2F, 0.8F, 1.0F, 1.0F),     ///< ICE
    vec4(0.6F, 0.8F, 1.0F, 0.8F),     ///< STEAM
    vec4(0.1F, 0.25F, 0.05F, 1.0F),   ///< OIL
    vec4(0.90F, 0.90F, 0.90F, 1.0F),  ///< GUNPOWDER
    vec4(0.75F, 0.75F, 0.2F, 1.0F),   ///< GASOLINE
    vec4(0.4F, 0.2F, 0.6F, 1.0F),     ///< METAL
};
constexpr unsigned int ATTRIBUTES[] = {
    INERT,                                              ///< AIR
    INERT,                                              ///< SAND
    FLAMMABLE,                                          ///< SAWDUST
    INERT,                                              ///< CONCRETE
    ON_FIRE,                                            ///< FIRE
    INERT,                                              ///< SMOKE
    TURNS_TO_SOLID& TURNS_TO_LIQUID& TURNS_TO_GAS& WET, ///< WATER
    TURNS_TO_SOLID& TURNS_TO_LIQUID& TURNS_TO_GAS& WET, ///< ICE
    TURNS_TO_SOLID& TURNS_TO_LIQUID& TURNS_TO_GAS& WET, ///< STEAM
    FLAMMABLE,                                          ///< OIL
    EXPLOSIVE,                                          ///< GUNPOWDER
    EXPLOSIVE,                                          ///< GASOLINE
    INERT,                                              ///< METAL
};

/////////////////////////////////////////////////////////////////////////
/// \struct Particle
struct Particle {
    int m_tickNum = 0;
    bool m_moveable = true;
    bool m_asleep = false;
    float m_density = 0.0F;
    float m_health = 1.0F;
    float m_mass = 0.0F;
    float m_thermalConductivity = 1.0F;
    float m_temp = ROOM_TEMP;
    float m_impulseAccumulator = 0.0F;
    int m_attributes = Attributes::INERT;
    MatterState m_state = MatterState::GAS;
    Element m_element = Element::AIR;
};
constexpr auto particleSize = sizeof(Particle);

/////////////////////////////////////////////////////////////////////////
/// \struct GPU_Particle
struct GPU_Particle {
    vec4 m_color = vec4(0);
    vec2 m_pos = vec2(0.0F);
    vec2 padding = vec2(0.0F);
};

/////////////////////////////////////////////////////////////////////////
/// \struct GPU_Particle
struct CellChunk {
    ivec2 m_begin;
    ivec2 m_end;
    CellChunk(
        const int& beginX, const int& beginY, const int& endX,
        const int& endY) noexcept
        : m_begin(beginX, beginY), m_end(endX, endY) {}
    CellChunk(const ivec2& begin, const ivec2& end) noexcept
        : m_begin(begin), m_end(end) {}
};
#endif // DEFINITIONS_HPP
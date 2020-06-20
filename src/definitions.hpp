#pragma once
#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include "Utility/vec.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// Cross-application definitions
constexpr int WIDTH = 1280;
constexpr int HEIGHT = 768;
constexpr int CELL_SIZE = 256;
constexpr double TIME_STEP = 0.025;
constexpr vec4 COLOR_CONCRETE = vec4(0.4F, 0.4F, 0.4F, 1);
constexpr vec4 COLOR_SLUDGE = vec4(0.15F, 0.15F, 0.15F, 1.0F);
constexpr vec4 COLOR_SAND = vec4(0.75F, 0.6F, 0.4F, 1.0F);
constexpr vec4 COLOR_OIL = vec4(0.1F, 0.25F, 0.05F, 1.0F);
constexpr vec4 COLOR_GUNPOWDER = vec4(0.90F, 0.90F, 0.90F, 1.0F);
constexpr vec4 COLOR_GASOLINE = vec4(0.75F, 0.75F, 0.2F, 1.0F);
constexpr vec4 COLOR_FIRE = vec4(1.0F, 0.2F, 0.0F, 1.0F);
constexpr vec4 COLOR_WATER = vec4(0.1F, 0.2F, 1.0F, 1.0F);
enum class MatterState { SOLID, LIQUID, GAS };

/////////////////////////////////////////////////////////////////////////
/// \class Particle
struct Particle {
    bool m_exists = false;
    bool m_moveable = true;
    bool m_asleep = false;
    bool m_onFire = false;
    MatterState m_state = MatterState::SOLID;
    float m_density = 0.0f;
    float m_health = 1.0F;
    vec4 m_color = vec4(0);
};
constexpr auto particle_size = sizeof(Particle);

/////////////////////////////////////////////////////////////////////////
/// \class GPU_Particle
struct GPU_Particle {
    vec4 m_color = vec4(0);
    vec2 m_pos = vec2(0.0F);
    int m_onFire = 0;
    float padding = 1.0f;
};

#endif // DEFINITIONS_HPP
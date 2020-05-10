#pragma once
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Utility/vec.hpp"

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// Particle Enumerations
enum class PARTICLE_TYPE : int { CONCRETE, SAND };

/////////////////////////////////////////////////////////////////////////
/// \class GPU_Particle
struct GPU_Particle {
    vec2 m_pos = vec2(0.0F);
    vec2 m_velocity = vec2(0.0F);
    vec2 m_scale = vec2(1.0F);
};

/////////////////////////////////////////////////////////////////////////
/// \class Game_Particle
struct Game_Particle {
    vec2 m_pos = vec2(0.0F);
    vec2 m_velocity = vec2(0.0F);
    PARTICLE_TYPE m_type = PARTICLE_TYPE::CONCRETE;
    float m_mass = 1.0F;
    float m_size = 1.0F;
};

#endif // PARTICLE_HPP
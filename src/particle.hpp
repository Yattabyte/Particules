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
    int m_type = 0;
    float padding;
};

#endif // PARTICLE_HPP
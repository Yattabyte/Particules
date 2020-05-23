#pragma once
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Utility/vec.hpp"
#include <array>

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

/////////////////////////////////////////////////////////////////////////
/// Particle Enumerations
#define COLOR_CONCRETE vec3(0.4F);
#define COLOR_SAND vec3(0.80F, 0.75F, 0.55F);
#define COLOR_FIRE vec3(1, 0.2F, 0);

/////////////////////////////////////////////////////////////////////////
/// \class GPU_Particle
struct GPU_Particle {
    vec3 m_color = vec3(1);
    float padding1 = 0;
    vec2 m_pos = vec2(0.0F);
    vec2 m_velocity = vec2(0.0F);
    vec2 m_scale = vec2(1);
    vec2 padding2 = vec2(0);
};

#endif // PARTICLE_HPP
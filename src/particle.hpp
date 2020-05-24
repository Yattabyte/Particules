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
#define COLOR_SLUDGE vec3(0.15F);
#define COLOR_SAND vec3(0.75F, 0.6F, 0.4F);
#define COLOR_OIL vec3(0.1F, 0.25F, 0.05F);
#define COLOR_GUNPOWDER vec3(0.90F);
#define COLOR_GASOLINE vec3(0.75F, 0.75F, 0.2F);
#define COLOR_FIRE vec3(1, 0.2F, 0);

/////////////////////////////////////////////////////////////////////////
/// \class GPU_Particle
struct GPU_Particle {
    vec3 m_color = vec3(1);
    int m_onFire = 0;
    vec2 m_pos = vec2(0.0F);
    vec2 m_scale = vec2(1);
};

#endif // PARTICLE_HPP
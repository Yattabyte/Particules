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
#define COLOR_CONCRETE vec4(0.4F, 0.4F, 0.4F, 1);
#define COLOR_SLUDGE vec4(0.15F, 0.15F, 0.15F, 1.0F);
#define COLOR_SAND vec4(0.75F, 0.6F, 0.4F, 1.0F);
#define COLOR_OIL vec4(0.1F, 0.25F, 0.05F, 1.0F);
#define COLOR_GUNPOWDER vec4(0.90F, 0.90F, 0.90F, 1.0F);
#define COLOR_GASOLINE vec4(0.75F, 0.75F, 0.2F, 1.0F);
#define COLOR_FIRE vec4(1.0F, 0.2F, 0.0F, 1.0F);
#define COLOR_WATER vec4(0.1F, 0.2F, 1.0F, 1.0F);

/////////////////////////////////////////////////////////////////////////
/// \class Particle
struct Particle {
    /////////////////////////////////////////////////////////////////////////
    bool m_exists = false;
    bool m_moveable = true;
    bool m_asleep = false;
    bool m_onFire = false;
    enum class MatterState { SOLID, LIQUID, GAS } m_state = MatterState::SOLID;
    float m_density = 0.0f;
    float m_health = 1.0F;
    vec4 m_color = vec4(0);
};
// constexpr auto size = sizeof(Particle);
/////////////////////////////////////////////////////////////////////////
/// \class GPU_Particle
struct GPU_Particle {
    vec4 m_color = vec4(0);
    vec2 m_pos = vec2(0.0F);
    int m_onFire = 0;
    float padding = 1.0f;
};

#endif // PARTICLE_HPP
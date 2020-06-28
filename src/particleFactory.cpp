#include "particleFactory.hpp"

ParticleFactory::ParticleFactory() {
    // Create AIR
    m_particleTypes[Element::AIR];

    // Create SAND
    m_particleTypes[Element::SAND].m_moveable = true;
    m_particleTypes[Element::SAND].m_health = 10.0F;
    m_particleTypes[Element::SAND].m_density = 1.0F;
    m_particleTypes[Element::SAND].m_state = MatterState::SOLID;
    m_particleTypes[Element::SAND].m_element = Element::SAND;
    m_particleTypes[Element::SAND].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::SAND)];

    // Create CONCRETE
    m_particleTypes[Element::CONCRETE].m_moveable = false;
    m_particleTypes[Element::CONCRETE].m_health = 1000.0F;
    m_particleTypes[Element::CONCRETE].m_density = 1000.0F;
    m_particleTypes[Element::CONCRETE].m_state = MatterState::SOLID;
    m_particleTypes[Element::CONCRETE].m_element = Element::CONCRETE;
    m_particleTypes[Element::CONCRETE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::CONCRETE)];

    // Create FIRE
    m_particleTypes[Element::FIRE].m_moveable = true;
    m_particleTypes[Element::FIRE].m_health = 25.0F;
    m_particleTypes[Element::FIRE].m_density = -0.1F;
    m_particleTypes[Element::FIRE].m_state = MatterState::GAS;
    m_particleTypes[Element::FIRE].m_element = Element::FIRE;
    m_particleTypes[Element::FIRE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::FIRE)];

    // Create SMOKE
    m_particleTypes[Element::SMOKE].m_moveable = true;
    m_particleTypes[Element::SMOKE].m_health = 50.0F;
    m_particleTypes[Element::SMOKE].m_density = -1.0F;
    m_particleTypes[Element::SMOKE].m_state = MatterState::GAS;
    m_particleTypes[Element::SMOKE].m_element = Element::SMOKE;
    m_particleTypes[Element::SMOKE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::SMOKE)];

    // Create WATER
    m_particleTypes[Element::WATER].m_moveable = true;
    m_particleTypes[Element::WATER].m_health = 1.0F;
    m_particleTypes[Element::WATER].m_density = 0.5F;
    m_particleTypes[Element::WATER].m_state = MatterState::LIQUID;
    m_particleTypes[Element::WATER].m_element = Element::WATER;
    m_particleTypes[Element::WATER].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::WATER)];

    // Create OIL
    m_particleTypes[Element::OIL].m_moveable = true;
    m_particleTypes[Element::OIL].m_health = 1.0F;
    m_particleTypes[Element::OIL].m_density = 0.4F;
    m_particleTypes[Element::OIL].m_state = MatterState::LIQUID;
    m_particleTypes[Element::OIL].m_element = Element::OIL;
    m_particleTypes[Element::OIL].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::OIL)];

    // Create GUNPOWDER
    m_particleTypes[Element::GUNPOWDER].m_moveable = true;
    m_particleTypes[Element::GUNPOWDER].m_health = 0.125F;
    m_particleTypes[Element::GUNPOWDER].m_density = 0.8F;
    m_particleTypes[Element::GUNPOWDER].m_state = MatterState::SOLID;
    m_particleTypes[Element::GUNPOWDER].m_element = Element::GUNPOWDER;
    m_particleTypes[Element::GUNPOWDER].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::GUNPOWDER)];

    // Create GASOLINE
    m_particleTypes[Element::GASOLINE].m_moveable = true;
    m_particleTypes[Element::GASOLINE].m_health = 100.0F;
    m_particleTypes[Element::GASOLINE].m_density = -0.2F;
    m_particleTypes[Element::GASOLINE].m_state = MatterState::GAS;
    m_particleTypes[Element::GASOLINE].m_element = Element::GASOLINE;
    m_particleTypes[Element::GASOLINE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::GASOLINE)];

    // Create METAL
    m_particleTypes[Element::METAL].m_moveable = true;
    m_particleTypes[Element::METAL].m_health = 150.0F;
    m_particleTypes[Element::METAL].m_density = 1000.0F;
    m_particleTypes[Element::METAL].m_state = MatterState::SOLID;
    m_particleTypes[Element::METAL].m_element = Element::METAL;
    m_particleTypes[Element::METAL].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::METAL)];
}

ParticleFactory& ParticleFactory::getInstance() {
    static ParticleFactory factory;
    return factory;
}

Particle ParticleFactory::makeType(const Element& element) {
    return getInstance().m_particleTypes[element];
}
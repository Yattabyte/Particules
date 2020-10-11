#include "particleFactory.hpp"

//////////////////////////////////////////////////////////////////////
/// ParticleFactory
//////////////////////////////////////////////////////////////////////

ParticleFactory::ParticleFactory() {
    // Create AIR
    m_particleTypes[Element::AIR];
    m_particleTypes[Element::AIR].m_moveable = true;
    m_particleTypes[Element::AIR].m_health = 1.0F;
    m_particleTypes[Element::AIR].m_density = 1.225F;
    m_particleTypes[Element::AIR].m_mass = 0.0F;
    m_particleTypes[Element::AIR].m_temp = ROOM_TEMP;
    m_particleTypes[Element::AIR].m_thermalConductivity = 0.0262F;
    m_particleTypes[Element::AIR].m_state = MatterState::GAS;
    m_particleTypes[Element::AIR].m_element = Element::AIR;
    m_particleTypes[Element::AIR].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::AIR)];

    // Create SAND
    m_particleTypes[Element::SAND].m_moveable = true;
    m_particleTypes[Element::SAND].m_health = 10.0F;
    m_particleTypes[Element::SAND].m_density = 1442.0F;
    m_particleTypes[Element::SAND].m_mass = 2.0F;
    m_particleTypes[Element::SAND].m_temp = ROOM_TEMP;
    m_particleTypes[Element::SAND].m_thermalConductivity = 0.25F;
    m_particleTypes[Element::SAND].m_state = MatterState::SOLID;
    m_particleTypes[Element::SAND].m_element = Element::SAND;
    m_particleTypes[Element::SAND].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::SAND)];

    // Create SAWDUST
    m_particleTypes[Element::SAWDUST].m_moveable = true;
    m_particleTypes[Element::SAWDUST].m_health = 65.0F;
    m_particleTypes[Element::SAWDUST].m_density = 210.0F;
    m_particleTypes[Element::SAWDUST].m_mass = 0.9F;
    m_particleTypes[Element::SAWDUST].m_temp = ROOM_TEMP;
    m_particleTypes[Element::SAWDUST].m_thermalConductivity = 0.08F;
    m_particleTypes[Element::SAWDUST].m_state = MatterState::SOLID;
    m_particleTypes[Element::SAWDUST].m_element = Element::SAWDUST;
    m_particleTypes[Element::SAWDUST].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::SAWDUST)];

    // Create CONCRETE
    m_particleTypes[Element::CONCRETE].m_moveable = false;
    m_particleTypes[Element::CONCRETE].m_health = 1000.0F;
    m_particleTypes[Element::CONCRETE].m_density = 1000.0F;
    m_particleTypes[Element::CONCRETE].m_mass = 0.0F;
    m_particleTypes[Element::CONCRETE].m_temp = ROOM_TEMP;
    m_particleTypes[Element::CONCRETE].m_thermalConductivity = 1.8F;
    m_particleTypes[Element::CONCRETE].m_state = MatterState::SOLID;
    m_particleTypes[Element::CONCRETE].m_element = Element::CONCRETE;
    m_particleTypes[Element::CONCRETE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::CONCRETE)];

    // Create FIRE
    m_particleTypes[Element::FIRE].m_moveable = true;
    m_particleTypes[Element::FIRE].m_health = 200.0F;
    m_particleTypes[Element::FIRE].m_density = -1.0F;
    m_particleTypes[Element::FIRE].m_mass = 2.0F;
    m_particleTypes[Element::FIRE].m_temp = 1000.0F;
    m_particleTypes[Element::FIRE].m_state = MatterState::GAS;
    m_particleTypes[Element::FIRE].m_element = Element::FIRE;
    m_particleTypes[Element::FIRE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::FIRE)];

    // Create SMOKE
    m_particleTypes[Element::SMOKE].m_moveable = true;
    m_particleTypes[Element::SMOKE].m_health = 60.0F;
    m_particleTypes[Element::SMOKE].m_density = 1.180F;
    m_particleTypes[Element::SMOKE].m_mass = 2.5F;
    m_particleTypes[Element::SMOKE].m_temp = 100.0F;
    m_particleTypes[Element::SMOKE].m_thermalConductivity = 0.0146F;
    m_particleTypes[Element::SMOKE].m_state = MatterState::GAS;
    m_particleTypes[Element::SMOKE].m_element = Element::SMOKE;
    m_particleTypes[Element::SMOKE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::SMOKE)];

    // Create WATER
    m_particleTypes[Element::WATER].m_moveable = true;
    m_particleTypes[Element::WATER].m_health = 500.0F;
    m_particleTypes[Element::WATER].m_density = 997.0F;
    m_particleTypes[Element::WATER].m_mass = 5.0F;
    m_particleTypes[Element::WATER].m_temp = ROOM_TEMP;
    m_particleTypes[Element::WATER].m_thermalConductivity = 0.609F;
    m_particleTypes[Element::WATER].m_state = MatterState::LIQUID;
    m_particleTypes[Element::WATER].m_element = Element::WATER;
    m_particleTypes[Element::WATER].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::WATER)];

    // Create SNOW
    m_particleTypes[Element::SNOW].m_moveable = true;
    m_particleTypes[Element::SNOW].m_health = 500.0F;
    m_particleTypes[Element::SNOW].m_density = 0.75F;
    m_particleTypes[Element::SNOW].m_mass = 1.0F;
    m_particleTypes[Element::SNOW].m_temp = -100.0f;
    m_particleTypes[Element::SNOW].m_thermalConductivity = 0.4F;
    m_particleTypes[Element::SNOW].m_state = MatterState::SOLID;
    m_particleTypes[Element::SNOW].m_element = Element::SNOW;
    m_particleTypes[Element::SNOW].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::SNOW)];

    // Create ICE
    m_particleTypes[Element::ICE].m_moveable = false;
    m_particleTypes[Element::ICE].m_health = 500.0F;
    m_particleTypes[Element::ICE].m_density = 0.90F;
    m_particleTypes[Element::ICE].m_mass = 0.0F;
    m_particleTypes[Element::ICE].m_temp = -100.0f;
    m_particleTypes[Element::ICE].m_thermalConductivity = 0.3045F;
    m_particleTypes[Element::ICE].m_state = MatterState::SOLID;
    m_particleTypes[Element::ICE].m_element = Element::ICE;
    m_particleTypes[Element::ICE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::ICE)];

    // Create STEAM
    m_particleTypes[Element::STEAM].m_moveable = true;
    m_particleTypes[Element::STEAM].m_health = 500.0F;
    m_particleTypes[Element::STEAM].m_density = 0.6F;
    m_particleTypes[Element::STEAM].m_mass = 5.0F;
    m_particleTypes[Element::STEAM].m_temp = 150.0F;
    m_particleTypes[Element::STEAM].m_thermalConductivity = 0.45675F;
    m_particleTypes[Element::STEAM].m_state = MatterState::GAS;
    m_particleTypes[Element::STEAM].m_element = Element::STEAM;
    m_particleTypes[Element::STEAM].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::STEAM)];

    // Create OIL
    m_particleTypes[Element::OIL].m_moveable = true;
    m_particleTypes[Element::OIL].m_health = 300.0F;
    m_particleTypes[Element::OIL].m_density = 900.0F;
    m_particleTypes[Element::OIL].m_mass = 6.0F;
    m_particleTypes[Element::OIL].m_temp = ROOM_TEMP;
    m_particleTypes[Element::OIL].m_thermalConductivity = 0.145F;
    m_particleTypes[Element::OIL].m_state = MatterState::LIQUID;
    m_particleTypes[Element::OIL].m_element = Element::OIL;
    m_particleTypes[Element::OIL].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::OIL)];

    // Create GUNPOWDER
    m_particleTypes[Element::GUNPOWDER].m_moveable = true;
    m_particleTypes[Element::GUNPOWDER].m_health = 35.0F;
    m_particleTypes[Element::GUNPOWDER].m_density = 400.0F;
    m_particleTypes[Element::GUNPOWDER].m_mass = 1.5F;
    m_particleTypes[Element::GUNPOWDER].m_temp = ROOM_TEMP;
    m_particleTypes[Element::GUNPOWDER].m_thermalConductivity = 0.18F;
    m_particleTypes[Element::GUNPOWDER].m_state = MatterState::SOLID;
    m_particleTypes[Element::GUNPOWDER].m_element = Element::GUNPOWDER;
    m_particleTypes[Element::GUNPOWDER].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::GUNPOWDER)];

    // Create GASOLINE
    m_particleTypes[Element::GASOLINE].m_moveable = true;
    m_particleTypes[Element::GASOLINE].m_health = 100.0F;
    m_particleTypes[Element::GASOLINE].m_density = 780.0F;
    m_particleTypes[Element::GASOLINE].m_mass = 0.8F;
    m_particleTypes[Element::GASOLINE].m_temp = ROOM_TEMP;
    m_particleTypes[Element::GASOLINE].m_thermalConductivity = 0.15F;
    m_particleTypes[Element::GASOLINE].m_state = MatterState::GAS;
    m_particleTypes[Element::GASOLINE].m_element = Element::GASOLINE;
    m_particleTypes[Element::GASOLINE].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::GASOLINE)];

    // Create METAL
    m_particleTypes[Element::METAL].m_moveable = true;
    m_particleTypes[Element::METAL].m_health = 150.0F;
    m_particleTypes[Element::METAL].m_density = 1000.0F;
    m_particleTypes[Element::METAL].m_mass = 0.0F;
    m_particleTypes[Element::METAL].m_temp = ROOM_TEMP;
    m_particleTypes[Element::METAL].m_thermalConductivity = 25.0F;
    m_particleTypes[Element::METAL].m_state = MatterState::SOLID;
    m_particleTypes[Element::METAL].m_element = Element::METAL;
    m_particleTypes[Element::METAL].m_attributes =
        ATTRIBUTES[static_cast<int>(Element::METAL)];
}

//////////////////////////////////////////////////////////////////////
/// getInstance
//////////////////////////////////////////////////////////////////////

ParticleFactory& ParticleFactory::getInstance() {
    static ParticleFactory factory;
    return factory;
}

//////////////////////////////////////////////////////////////////////
/// makeType
//////////////////////////////////////////////////////////////////////

Particle ParticleFactory::makeType(const Element& element) {
    return getInstance().m_particleTypes[element];
}
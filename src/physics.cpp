#include "physics.hpp"
#include "particleFactory.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <array>

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

Physics::Physics(
    std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>& particles) noexcept
    : m_particles(particles),
      m_previousTemps(std::shared_ptr<float[HEIGHT + 1][WIDTH + 1]>(new float[HEIGHT + 1][WIDTH + 1])) {
    // Default air temperature to 0
    for (auto n = 0; n < HEIGHT + 1; ++n)
        for (auto m = 0; m < WIDTH + 1; ++m)
            m_previousTemps[n][m] = ROOM_TEMP;
}

//////////////////////////////////////////////////////////////////////
/// simulate
//////////////////////////////////////////////////////////////////////

void Physics::simulate(const int tickNum, const ivec2& begin, const ivec2& end) noexcept {
    // Copy previous temperatures
    for (int y = begin.y(); y < end.y(); ++y)
        for (int x = begin.x(); x < end.x(); ++x)
            m_previousTemps[y][x] = getParticle(ivec2(x, y)).m_temp;

    // Apply Gravity
    for (int y = begin.y(); y < end.y(); ++y) {
        for (int x = begin.x(); x < end.x(); ++x) {
            ivec2 coords(x, y);
            auto& particle = getParticle(coords);

            // Avoid acting twice on a particle per tick in case they move or are spawned
            if (particle.m_tickNum >= tickNum) {
                continue;
            }
            particle.m_tickNum = tickNum;

            // Perform heat-transfer simulation
            simulateHeat(coords);

            // Air is neutral, only simulate heat through it
            if (particle.m_element == Element::AIR) {
                continue;
            }

            // Delete based on low-health
            else if (particle.m_health <= 0.0F) {
                spawnParticle(Element::AIR, coords);
                continue;
            }

            // Simulate the particle n times (based on its mass)
            const Element elementCopy = particle.m_element;
            simulateState(coords);

            // Simulate interactions based on element
            simulateElement(elementCopy, coords);

            // Simulate attributes like on fire, ignition, etc
            simulateAttributes(coords);
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// simulateState_*
//////////////////////////////////////////////////////////////////////

void Physics::simulateState(ivec2& coords) noexcept {
    Particle& particle = getParticle(coords);
    if (!particle.m_moveable || particle.m_asleep) {
        return;
    }

    // Retrieve the amount of times this particle should move
    particle.m_impulseAccumulator += particle.m_mass;
    int numberOfIterations = static_cast<int>(particle.m_impulseAccumulator);
    particle.m_impulseAccumulator -= numberOfIterations;

    // Perform different simulations based on matter type of particle
    switch (particle.m_state) {
    case MatterState::SOLID:
        for (int x = 0; x < numberOfIterations; ++x) {
            if (!simulateState_Solid(coords))
                return;
        }
        break;
    case MatterState::LIQUID:
        for (int x = 0; x < numberOfIterations; ++x) {
            if (!simulateState_Liquid(coords))
                return;
        }
        break;
    case MatterState::GAS:
        for (int x = 0; x < numberOfIterations; ++x) {
            if (!simulateState_Gas(coords))
                return;
        }
        break;
    }
}

//////////////////////////////////////////////////////////////////////

bool Physics::simulateState_Solid(
    ivec2& coords) noexcept {
    constexpr std::array<std::array<ivec2, 3>, 2> offsets = {
        std::array<ivec2, 3>{ ivec2{ 0, -1 }, { -1, -1 }, { 1, -1 } },
        std::array<ivec2, 3>{ ivec2{ 0, -1 }, { 1, -1 }, { -1, -1 } },
    };
    const auto& directions = offsets[fastRand() % 2];

    const auto& particle = getParticle(coords);
    for (const auto& offset : directions) {
        if (coords.x() + offset.x() < 0 || coords.y() + offset.y() < 0)
            continue;

        const auto& targetParticle = getParticle(coords + offset);
        if (!targetParticle.m_moveable || targetParticle.m_density >= particle.m_density)
            continue;

        swapTile(coords, coords + offset);
        coords += offset;
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////

bool Physics::simulateState_Liquid(
    ivec2& coords) noexcept {
    constexpr std::array<std::array<ivec2, 5>, 4> offsets = {
        std::array<ivec2, 5>{
            ivec2{ 0, -1 }, { -1, -1 }, { 1, -1 }, { -1, 0 }, { 1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, -1 }, { 1, -1 }, { -1, -1 }, { 1, 0 }, { -1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, -1 }, { -1, -1 }, { 1, -1 }, { 1, 0 }, { -1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, -1 }, { 1, -1 }, { -1, -1 }, { -1, 0 }, { 1, 0 } },
    };

    const auto& directions = offsets[fastRand() % 4];
    const auto& particle = getParticle(coords);
    for (const auto& offset : directions) {
        if (coords.x() + offset.x() < 0 || coords.y() + offset.y() < 0)
            continue;
        const auto& targetParticle = getParticle(coords + offset);
        if (!targetParticle.m_moveable || targetParticle.m_density >= particle.m_density)
            continue;

        swapTile(coords, coords + offset);
        coords += offset;
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////

bool Physics::simulateState_Gas(ivec2& coords) noexcept {
    constexpr std::array<std::array<ivec2, 5>, 4> offsets = {
        std::array<ivec2, 5>{
            ivec2{ 0, 1 }, { -1, 1 }, { 1, 1 }, { -1, 0 }, { 1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, 1 }, { 1, 1 }, { -1, 1 }, { 1, 0 }, { -1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, 1 }, { -1, 1 }, { 1, 1 }, { 1, 0 }, { -1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, 1 }, { 1, 1 }, { -1, 1 }, { -1, 0 }, { 1, 0 } }
    };

    // Tick down its health as it disperses itself
    auto& particle = getParticle(coords);
    particle.m_health -= 1.0F;

    const auto& directions = offsets[fastRand() % 4];
    for (const auto& offset : directions) {
        if (coords.x() + offset.x() < 0 || coords.y() + offset.y() < 0)
            continue;

        const auto& targetParticle = getParticle(coords + offset);
        if (!targetParticle.m_moveable || targetParticle.m_density < particle.m_density)
            continue;

        swapTile(coords, coords + offset);
        coords += offset;
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////
/// simulateElement_*
//////////////////////////////////////////////////////////////////////

void Physics::simulateElement(const Element element, const ivec2& coords) noexcept {
    switch (element) {
    case Element::AIR:
        simulateElement_AIR(coords);
        break;
    case Element::SAND:
        simulateElement_SAND(coords);
        break;
    case Element::SAWDUST:
        simulateElement_SAWDUST(coords);
        break;
    case Element::CONCRETE:
        simulateElement_CONCRETE(coords);
        break;
    case Element::FIRE:
        simulateElement_FIRE(coords);
        break;
    case Element::SMOKE:
        simulateElement_SMOKE(coords);
        break;
    case Element::WATER:
        simulateElement_WATER(coords);
        break;
    case Element::SNOW:
        simulateElement_SNOW(coords);
        break;
    case Element::ICE:
        simulateElement_ICE(coords);
        break;
    case Element::STEAM:
        simulateElement_STEAM(coords);
        break;
    case Element::OIL:
        simulateElement_OIL(coords);
        break;
    case Element::GUNPOWDER:
        simulateElement_GUNPOWDER(coords);
        break;
    case Element::GASOLINE:
        simulateElement_GASOLINE(coords);
        break;
    case Element::METAL:
        simulateElement_METAL(coords);
        break;
    }
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_AIR(const ivec2& /*coords*/) noexcept {
    // INERT
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_SAND(const ivec2& /*coords*/) noexcept {
    // INERT
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_SAWDUST(const ivec2& /*coords*/) noexcept {
    // FLAMMABLE
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_CONCRETE(const ivec2& /*coords*/) noexcept {
    // INERT
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_FIRE(const ivec2& coords) noexcept {
    // Tick down its health
    auto& particle = getParticle(coords);
    particle.m_health -= 1.0F;
    // IGNITES
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_SMOKE(const ivec2& coords) noexcept {
    // Tick down its health
    auto& particle = getParticle(coords);
    particle.m_health -= 1.0F;
    // INERT
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_WATER(const ivec2& coords) noexcept {
    // Turn to ice
    auto& particle = getParticle(coords);
    if (particle.m_temp < 0.0F)
        spawnParticleKeepTemp(Element::ICE, coords);
    // Turn to steam
    else if (particle.m_temp > 100.0F)
        spawnParticleKeepTemp(Element::STEAM, coords);
    // DOUSES
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_SNOW(const ivec2& coords) noexcept {
    // Turn to ice
    auto& particle = getParticle(coords);
    if (particle.m_temp > 0.0F)
        spawnParticleKeepTemp(Element::WATER, coords);
    // DOUSES
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_ICE(const ivec2& coords) noexcept {
    // Turn to water
    auto& particle = getParticle(coords);
    if (particle.m_temp > 0.0F)
        spawnParticleKeepTemp(Element::WATER, coords);

    // DOUSES
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_STEAM(const ivec2& coords) noexcept {
    // Turn to water
    auto& particle = getParticle(coords);
    if (particle.m_temp < 100.0F)
        spawnParticleKeepTemp(Element::WATER, coords);
    // DOUSES
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_OIL(const ivec2& /*coords*/) noexcept {
    // FLAMMABLE
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_GUNPOWDER(const ivec2& /*coords*/) noexcept {
    // EXPLOSIVE
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_GASOLINE(const ivec2& /*coords*/) noexcept {
    // EXPLOSIVE
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_METAL(const ivec2& /*coords*/) noexcept {
    // INERT
}

//////////////////////////////////////////////////////////////////////
/// simulateAttributes
//////////////////////////////////////////////////////////////////////

void Physics::simulateAttributes(const ivec2& coords) noexcept {
    //constexpr std::array<ivec2, 8> directions = { ivec2{ 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
    constexpr std::array<ivec2, 4> directions = { ivec2{ -1, 0 }, { 0, -1 }, { 0, 1 }, { 1, 0 } };
    auto& particle = getParticle(coords);

    // Skip if this particle does nothing or has no condition set
    if (particle.hasAttribute(Attributes::INERT))
        return;

    // Retrieve a random number for this attribute simulation
    const auto randomNumber = fastRand();

    // Burning particle
    if (particle.hasAttribute(Attributes::IGNITES)) {
        // Tick down its health as it expends itself
        particle.m_health -= 1.0F;

        // Ignite a random adjacent cell
        const auto& direction = directions[randomNumber % 4];
        if (coords.x() + direction.x() >= 0 && coords.y() + direction.y() >= 0) {
            auto& adjacentParticle = getParticle(coords + directions[randomNumber % 4]);

            // If adjacent particle is flammable, ignite it
            if (adjacentParticle.hasAttribute(Attributes::FLAMMABLE)) {
                adjacentParticle.m_attributes |= Attributes::IGNITES;
                adjacentParticle.m_asleep = false;
            }
        }

        if (randomNumber % 1000 >= 996) {
            if (randomNumber % 500 > 250)
                spawnParticle(Element::SMOKE, coords + direction);
            else
                spawnParticle(Element::FIRE, coords + direction);
        }
    } /*else if (particle.hasAttribute(Attributes::DOUSES)) {
        // Tick down its health as it expends itself
        particle.m_health -= 1.0F;

        // Douse fires on each direction
        for (const auto& direction : directions) {
            if (coords.x() + direction.x() < 0 || coords.y() + direction.y() < 0)
                continue;

            auto& adjacentParticle = getParticle(coords + direction);
            if (adjacentParticle.hasAttribute(Attributes::IGNITES)) {
                adjacentParticle.m_attributes ^= Attributes::IGNITES;
                adjacentParticle.m_asleep = false;
            }
        }
    }*/
}

//////////////////////////////////////////////////////////////////////
/// simulateHeat
//////////////////////////////////////////////////////////////////////

void Physics::simulateHeat(const ivec2& coords) noexcept {
    //constexpr ivec2 offsets[8] = { ivec2{ -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
    constexpr ivec2 offsets[4] = { ivec2{ -1, 0 }, { 0, -1 }, { 0, 1 }, { 1, 0 } };

    auto& particle = getParticle(coords);
    float adjacentDeltaHeat = 0.0F;
    for (const auto& offset : offsets) {
        const ivec2 finalCoords = coords + offset;
        if (finalCoords.x() <= 0 || finalCoords.x() >= WIDTH - 1 || finalCoords.y() <= 0 || finalCoords.y() >= HEIGHT - 1)
            adjacentDeltaHeat -= (m_previousTemps[coords.y()][coords.x()] / 4);
        else
            adjacentDeltaHeat -= (m_previousTemps[coords.y()][coords.x()] - m_previousTemps[finalCoords.y()][finalCoords.x()]);
    }
    particle.m_temp += 0.001F * (particle.m_thermalConductivity * 1000.0F * static_cast<float>(TIME_STEP)) * adjacentDeltaHeat;
    particle.m_temp = std::clamp(particle.m_temp, -1000.0F, 1000000.0F);
}

//////////////////////////////////////////////////////////////////////
/// spawnParticle
//////////////////////////////////////////////////////////////////////

void Physics::spawnParticle(const Element elementType, const ivec2& coords) {
    auto& particle = getParticle(coords);
    const int tickCopy = particle.m_tickNum;
    particle = ParticleFactory::makeType(elementType);
    particle.m_tickNum = tickCopy;
    m_previousTemps[coords.y()][coords.x()] = particle.m_temp;
}

//////////////////////////////////////////////////////////////////////
/// spawnParticleKeepTemp
//////////////////////////////////////////////////////////////////////

void Physics::spawnParticleKeepTemp(const Element elementType, const ivec2& coords) {
    auto& particle = getParticle(coords);
    const int tickCopy = particle.m_tickNum;
    const float tempCopy = particle.m_temp;
    particle = ParticleFactory::makeType(elementType);
    particle.m_tickNum = tickCopy;
}

//////////////////////////////////////////////////////////////////////
/// swapTile
//////////////////////////////////////////////////////////////////////

Particle& Physics::getParticle(const ivec2& coords) {
    return m_particles[coords.y()][coords.x()];
}

//////////////////////////////////////////////////////////////////////
/// swapTile
//////////////////////////////////////////////////////////////////////

void Physics::swapTile(const ivec2& coordA, const ivec2& coordB) noexcept {
    auto& particleA = getParticle(coordA);
    auto& particleB = getParticle(coordB);
    particleA.m_asleep = false;
    particleB.m_asleep = false;
    std::swap(particleA, particleB);
    std::swap(m_previousTemps[coordA.y()][coordA.x()], m_previousTemps[coordB.y()][coordB.x()]);
}
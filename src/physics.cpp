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
            auto* particle = &getParticle(coords);

            // Avoid acting twice on a particle per tick in case they move or are spawned
            if (particle->m_tickNum >= tickNum)
                continue;
            particle->m_tickNum = tickNum;

            // Perform heat-transfer simulation
            simulateHeat(coords);

            // Air is neutral, only simulate heat through it
            if (particle->m_element == Element::AIR)
                continue;

            // Delete based on low-health
            if (particle->m_health <= 0.0F) {
                spawnParticle(Element::AIR, coords);
                continue;
            }

            // Simulate the particle n times (based on its mass)
            const Element elementCopy = particle->m_element;
            coords = simulateState(coords);

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

ivec2 Physics::simulateState(const ivec2& coords) noexcept {
    auto* particle = &getParticle(coords);
    particle->m_impulseAccumulator += particle->m_mass;
    const auto timesToMove = static_cast<int>(particle->m_impulseAccumulator);

    ivec2 newCoords(coords);
    while ((particle->m_impulseAccumulator + 0.0001F) > 1.0F) {
        // Simulate movement based on matter-state
        if (particle->m_moveable && !particle->m_asleep) {
            switch (particle->m_state) {
            default:
            case MatterState::SOLID:
                newCoords = simulateState_Solid(newCoords);
                break;
            case MatterState::LIQUID:
                newCoords = simulateState_Liquid(newCoords);
                break;
            case MatterState::GAS:
                newCoords = simulateState_Gas(newCoords);
                break;
            }
            particle = &getParticle(newCoords);
            particle->m_impulseAccumulator -= 1.0F;
        }
    }
    return newCoords;
}

//////////////////////////////////////////////////////////////////////

ivec2 Physics::simulateState_Solid(
    const ivec2& coords) noexcept {
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
        return coords + offset;
    }
    return coords;
}

//////////////////////////////////////////////////////////////////////

ivec2 Physics::simulateState_Liquid(
    const ivec2& coords) noexcept {
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
        return coords + offset;
    }
    return coords;
}

//////////////////////////////////////////////////////////////////////

ivec2 Physics::simulateState_Gas(const ivec2& coords) noexcept {
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
    const auto& directions = offsets[fastRand() % 4];

    auto& particle = getParticle(coords);
    // Tick down its health as it disperses itself
    particle.m_health -= 1.0F;

    for (const auto& offset : directions) {
        if (coords.x() + offset.x() < 0 || coords.y() + offset.y() < 0)
            continue;

        const auto& targetParticle = getParticle(coords + offset);
        if (!targetParticle.m_moveable || targetParticle.m_density < particle.m_density)
            continue;

        swapTile(coords, coords + offset);
        return coords + offset;
    }
    return coords;
}

//////////////////////////////////////////////////////////////////////
/// simulateElement_*
//////////////////////////////////////////////////////////////////////

void Physics::simulateElement(const Element element, const ivec2& coords) noexcept {
    switch (element) {
    case Element::AIR:
        simulateElement_Air(coords);
        break;
    case Element::SAND:
        simulateElement_Sand(coords);
        break;
    case Element::CONCRETE:
        simulateElement_Concrete(coords);
        break;
    case Element::FIRE:
        simulateElement_Fire(coords);
        break;
    case Element::SMOKE:
        simulateElement_Smoke(coords);
        break;
    case Element::WATER:
        simulateElement_Water(coords);
        break;
    case Element::ICE:
        simulateElement_Ice(coords);
        break;
    case Element::STEAM:
        simulateElement_Steam(coords);
        break;
    }
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Air(const ivec2& /*coords*/) noexcept {
    // Inert
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Sand(const ivec2& /*coords*/) noexcept {
    // Inert
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Concrete(const ivec2& /*coords*/) noexcept {
    // Inert
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Fire(const ivec2& coords) noexcept {
    // Tick down its health
    auto& particle = getParticle(coords);
    particle.m_health -= 1.0F;
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Smoke(const ivec2& coords) noexcept {
    // Tick down its health
    auto& particle = getParticle(coords);
    particle.m_health -= 1.0F;
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Water(const ivec2& coords) noexcept {
    // Turn to ice
    auto& particle = getParticle(coords);
    if (particle.m_temp < 0.0F)
        spawnParticle(Element::ICE, coords);
    // Turn to steam
    else if (particle.m_temp > 100.0F)
        spawnParticle(Element::STEAM, coords);
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Ice(const ivec2& coords) noexcept {
    // Turn to water
    auto& particle = getParticle(coords);
    if (particle.m_temp > 0.0F)
        spawnParticle(Element::WATER, coords);
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Steam(const ivec2& coords) noexcept {
    // Turn to water
    auto& particle = getParticle(coords);
    if (particle.m_temp < 100.0F)
        spawnParticle(Element::WATER, coords);
}

//////////////////////////////////////////////////////////////////////
/// simulateAttributes
//////////////////////////////////////////////////////////////////////

void Physics::simulateAttributes(const ivec2& coords) noexcept {
    constexpr std::array<ivec2, 8> directions = { ivec2{ 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
    auto& particle = getParticle(coords);

    // Skip if this particle does nothing or has no condition set
    if ((particle.m_attributes & Attributes::INERT) == Attributes::INERT)
        return;

    // Retrieve a random number for this attribute simulation
    const auto randomNumber = fastRand();

    // Burning particle
    if ((particle.m_attributes & Attributes::ON_FIRE) == Attributes::ON_FIRE) {
        // Tick down its health as it expends itself
        particle.m_health -= 1.0F;

        // Ignite a random adjacent cell
        const auto& direction = directions[randomNumber % 8];
        if (coords.x() + direction.x() >= 0 && coords.y() + direction.y() >= 0) {
            auto& adjacentParticle = getParticle(coords + directions[randomNumber % 8]);

            // If adjacent particle is flammable, ignite it
            if ((adjacentParticle.m_attributes & Attributes::FLAMMABLE) == Attributes::FLAMMABLE) {
                adjacentParticle.m_attributes |= Attributes::ON_FIRE;
                adjacentParticle.m_asleep = false;
            }
        }

        if (randomNumber % 1000 >= 996) {
            if (randomNumber % 500 > 250)
                spawnParticle(Element::SMOKE, coords + direction);
            else
                spawnParticle(Element::FIRE, coords + direction);
        }
    } /*else if ((particle.m_attributes & Attributes::WET) == Attributes::WET) {
        // Tick down its health as it expends itself
        particle.m_health -= 1.0F;

        // Douse fires on each direction
        for (const auto& direction : directions) {
            if (coords.x() + direction.x() < 0 || coords.y() + direction.y() < 0)
                continue;

            auto& adjacentParticle = getParticle(coords + direction);
            if ((adjacentParticle.m_attributes & Attributes::ON_FIRE) ==
                Attributes::ON_FIRE) {
                adjacentParticle.m_attributes ^= Attributes::ON_FIRE;
                adjacentParticle.m_asleep = false;
            }
        }
    }*/
}

//////////////////////////////////////////////////////////////////////
/// simulateHeat
//////////////////////////////////////////////////////////////////////

void Physics::simulateHeat(const ivec2& coords) noexcept {
    auto& particle = getParticle(coords);
    float adjacentDeltaHeat = 0.0F;

    //constexpr ivec2 offsets[8] = { ivec2{ -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
    constexpr ivec2 offsets[4] = { ivec2{ -1, 0 }, { 0, -1 }, { 0, 1 }, { 1, 0 } };

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
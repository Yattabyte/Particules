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
    : m_particles(particles) {}

//////////////////////////////////////////////////////////////////////
/// simulate
//////////////////////////////////////////////////////////////////////

void Physics::simulate(
    const double& deltaTime, const int& tickNum, const int& beginX,
    const int& beginY, const int& endX, const int& endY) noexcept {
    // Apply Gravity
    for (int y = beginY; y < endY; ++y) {
        for (int x = beginX; x < endX; ++x) {
            int particleX = x;
            int particleY = y;
            auto* particle = &m_particles[particleY][particleX];

            ///\todo need to still do heat transfer
            // so change this later
            if (particle->m_element == Element::AIR)
                continue;

            // Avoid acting twice on a particle per tick
            // in case they move or are spawned
            if (particle->m_tickNum >= tickNum)
                continue;
            particle->m_tickNum = tickNum;

            // Simulate the particle n times (based on its mass)
            particle->m_impulseAccumulator += particle->m_mass;
            const auto timesToMove =
                static_cast<int>(particle->m_impulseAccumulator);
            ivec2 particlePos{ x, y };
            while (particle->m_impulseAccumulator + 0.0001F > 1.0F) {
                // Delete based on low-health
                if (particle->m_health <= 0.0F) {
                    *particle = ParticleFactory::makeType(Element::AIR);
                    continue;
                }

                // Simulate movement based on matter-state
                if (particle->m_moveable && !particle->m_asleep) {
                    switch (particle->m_state) {
                    default:
                    case MatterState::SOLID:
                        particlePos = simulateState_Solid(
                            deltaTime, particlePos.x, particlePos.y);
                        break;
                    case MatterState::LIQUID:
                        particlePos = simulateState_Liquid(
                            deltaTime, particlePos.x, particlePos.y);
                        break;
                    case MatterState::GAS:
                        particlePos = simulateState_Gas(
                            deltaTime, particlePos.x, particlePos.y);
                        break;
                    }
                    particleX = particlePos.x;
                    particleY = particlePos.y;
                    particle = &m_particles[particleY][particleX];
                    particle->m_impulseAccumulator -= 1.0F;
                }

                // Simulate state change next with heat simulation
                // simulateStateChange(x, y);

                // Simulate interactions based on element
                switch (particle->m_element) {
                case Element::AIR:
                    simulateElement_Air(particleX, particleY);
                    break;
                case Element::SAND:
                    simulateElement_Sand(particleX, particleY);
                    break;
                case Element::CONCRETE:
                    simulateElement_Concrete(particleX, particleY);
                    break;
                case Element::FIRE:
                    simulateElement_Fire(particleX, particleY);
                    break;
                case Element::SMOKE:
                    simulateElement_Smoke(particleX, particleY);
                    break;
                case Element::WATER:
                    simulateElement_Water(particleX, particleY);
                    break;
                }

                simulateAttributes(particleX, particleY);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// simulateState_*
//////////////////////////////////////////////////////////////////////

ivec2 Physics::simulateState_Solid(
    const double& /*deltaTime*/, const int& x, const int& y) noexcept {
    constexpr std::array<std::array<ivec2, 3>, 2> offsets = {
        std::array<ivec2, 3>{ ivec2{ 0, -1 }, { -1, -1 }, { 1, -1 } },
        std::array<ivec2, 3>{ ivec2{ 0, -1 }, { 1, -1 }, { -1, -1 } },
    };
    const auto directions = offsets[fastRand() % 2];

    auto& particle = m_particles[y][x];
    for (const auto& offset : directions) {
        if (x + offset.x < 0 || y + offset.y < 0)
            continue;

        auto& targetParticle = m_particles[y + offset.y][x + offset.x];
        if (!targetParticle.m_moveable ||
            targetParticle.m_density >= particle.m_density)
            continue;

        swapTile(particle, targetParticle);
        return ivec2{ x + offset.x, y + offset.y };
    }
    return ivec2{ x, y };
}

//////////////////////////////////////////////////////////////////////

ivec2 Physics::simulateState_Liquid(
    const double& /*deltaTime*/, const int& x, const int& y) noexcept {
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

    const auto directions = offsets[fastRand() % 4];
    auto& particle = m_particles[y][x];
    for (const auto& offset : directions) {
        if (x + offset.x < 0 || y + offset.y < 0)
            continue;
        auto& targetParticle = m_particles[y + offset.y][x + offset.x];
        if (!targetParticle.m_moveable ||
            targetParticle.m_density >= particle.m_density)
            continue;

        swapTile(particle, targetParticle);
        return ivec2{ x + offset.x, y + offset.y };
    }
    return ivec2{ x, y };
}

//////////////////////////////////////////////////////////////////////

ivec2 Physics::simulateState_Gas(
    const double& /*deltaTime*/, const int& x, const int& y) noexcept {
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
    const auto directions = offsets[fastRand() % 4];

    auto& particle = m_particles[y][x];
    for (const auto& offset : directions) {
        if (x + offset.x < 0 || y + offset.y < 0)
            continue;

        auto& targetParticle = m_particles[y + offset.y][x + offset.x];
        if (!targetParticle.m_moveable ||
            targetParticle.m_density <= particle.m_density)
            continue;

        swapTile(particle, targetParticle);
        return ivec2{ x + offset.x, y + offset.y };
    }
    return ivec2{ x, y };
}

//////////////////////////////////////////////////////////////////////
/// simulateElement_*
//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Air(const int& /*x*/, const int& /*y*/) noexcept {
    // Inert
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Sand(
    const int& /*x*/, const int& /*y*/) noexcept {
    // Inert
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Concrete(
    const int& /*x*/, const int& /*y*/) noexcept {
    // Inert
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Fire(const int& x, const int& y) noexcept {
    // Tick down its health
    auto& particle = m_particles[y][x];
    particle.m_health -= 1.0F;
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Smoke(const int& x, const int& y) noexcept {
    // Tick down its health
    auto& particle = m_particles[y][x];
    particle.m_health -= 100.0F;
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Water(const int& x, const int& y) noexcept {
    constexpr std::array<ivec2, 8> directions = { ivec2{ 0, 1 }, { -1, 1 },
                                                  { -1, 0 },     { -1, -1 },
                                                  { 0, -1 },     { 1, -1 },
                                                  { 1, 0 },      { 1, 1 } };
    // Douse fires on each direction
    auto& particle = m_particles[y][x];
    for (const auto& direction : directions) {
        if (x + direction.x < 0 || y + direction.y < 0)
            continue;
        Particle& adjacentParticle =
            m_particles[y + direction.y][x + direction.x];
        if ((adjacentParticle.m_attributes & Attributes::ON_FIRE) ==
            Attributes::ON_FIRE) {
            adjacentParticle.m_attributes ^= Attributes::ON_FIRE;
            particle.m_health -= 1.0F;
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// simulateAttributes
//////////////////////////////////////////////////////////////////////

void Physics::simulateAttributes(const int& x, const int& y) noexcept {
    auto& particle = m_particles[y][x];

    if ((particle.m_attributes & Attributes::ON_FIRE) == Attributes::ON_FIRE) {
        constexpr std::array<ivec2, 8> directions = { ivec2{ 0, 1 }, { -1, 1 },
                                                      { -1, 0 },     { -1, -1 },
                                                      { 0, -1 },     { 1, -1 },
                                                      { 1, 0 },      { 1, 1 } };
        // Tick down its health
        particle.m_health -= 1.0F;

        // 1/5 chance of igniting surroundings
        if (fastRand() % 10 > 7) {
            for (const auto& direction : directions) {
                if (x + direction.x < 0 || y + direction.y < 0)
                    continue;
                Particle& adjacentParticle =
                    m_particles[y + direction.y][x + direction.x];
                if (fastRand() % 10 > 8) {
                    if ((adjacentParticle.m_attributes &
                         Attributes::FLAMMABLE) == Attributes::FLAMMABLE) {
                        adjacentParticle.m_attributes |= Attributes::ON_FIRE;
                        adjacentParticle.m_asleep = false;
                    }
                }
            }
        }

        // Check if target has room to spawn stuff
        const auto& direction = directions[fastRand() % 8];
        if (x + direction.x < 0 || y + direction.y < 0)
            return;
        Particle& adjacentParticle =
            m_particles[y + direction.y][x + direction.x];
        if (adjacentParticle.m_element != Element::AIR)
            return;

        // 2/10 chance of making smoke
        if (fastRand() % 1000 >= 996) {
            adjacentParticle = ParticleFactory::makeType(Element::SMOKE);
        }

        // 2/10 chance of creating an ember
        if (fastRand() % 1000 >= 996) {
            adjacentParticle = ParticleFactory::makeType(Element::FIRE);
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// swapTile
//////////////////////////////////////////////////////////////////////

void Physics::swapTile(Particle& particleA, Particle& particleB) noexcept {
    std::swap(particleA, particleB);
    particleA.m_asleep = false;
    particleB.m_asleep = false;
}
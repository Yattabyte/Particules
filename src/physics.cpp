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
    const double& /*deltaTime*/, const int& tickNum, const int& beginX,
    const int& beginY, const int& endX, const int& endY) noexcept {
    // Apply Gravity
    for (int y = beginY; y < endY; ++y) {
        for (int x = beginX; x < endX; ++x) {
            auto& particle = m_particles[y][x];

            ///\todo need to still do heat transfer
            // so change this later
            if (particle.m_element == Element::AIR)
                continue;

            // Avoid acting twice on a particle per tick
            // in case they move or are spawned
            if (particle.m_tickNum >= tickNum)
                continue;
            particle.m_tickNum = tickNum;

            // Delete based on low-health
            if (particle.m_health <= 0.0F) {
                particle = ParticleFactory::makeType(Element::AIR);
            }

            // Simulate movement based on matter-state
            if (particle.m_moveable && !particle.m_asleep) {
                switch (particle.m_state) {
                default:
                case MatterState::SOLID:
                    simulateState_Solid(x, y);
                    break;
                case MatterState::LIQUID:
                    simulateState_Liquid(x, y);
                    break;
                case MatterState::GAS:
                    simulateState_Gas(x, y);
                    break;
                }
            }

            // Simulate state change next with heat simulation
            // simulateStateChange(x, y);

            // Simulate interactions based on element
            switch (particle.m_element) {
            default:
            case Element::AIR:
                simulateElement_Air(x, y);
                break;
            case Element::SAND:
                simulateElement_Sand(x, y);
                break;
            case Element::CONCRETE:
                simulateElement_Concrete(x, y);
                break;
            case Element::FIRE:
                simulateElement_Fire(x, y);
                break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// simulateState_*
//////////////////////////////////////////////////////////////////////

void Physics::simulateState_Solid(const int& x, const int& y) noexcept {
    struct ivec2 {
        int x, y;
    };
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
        break;
    }
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateState_Liquid(const int& x, const int& y) noexcept {
    struct ivec2 {
        int x, y;
    };
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
        break;
    }
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateState_Gas(const int& x, const int& y) noexcept {
    struct ivec2 {
        int x, y;
    };
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
    particle.m_density -= 0.1F; // gasses disperse vertically
    for (const auto& offset : directions) {
        if (x + offset.x < 0 || y + offset.y < 0)
            continue;

        auto& targetParticle = m_particles[y + offset.y][x + offset.x];
        if (!targetParticle.m_moveable ||
            targetParticle.m_density <= particle.m_density)
            continue;

        swapTile(particle, targetParticle);
        break;
    }
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
    struct ivec2 {
        int x, y;
    };
    constexpr std::array<ivec2, 8> directions = { ivec2{ 0, 1 }, { -1, 1 },
                                                  { -1, 0 },     { -1, -1 },
                                                  { 0, -1 },     { 1, -1 },
                                                  { 1, 0 },      { 1, 1 } };
    // Tick down its health
    auto& particle = m_particles[y][x];
    particle.m_health -= 1.0F;
    particle.m_density -= 0.1F;
    // particle.m_density = std::max(particle.m_density, -0.5F);

    // 1/5 chance of igniting surroundings
    if (fastRand() % 10 > 7) {
        for (const auto& direction : directions) {
            if (x + direction.x < 0 || y + direction.y < 0)
                return;
            Particle& adjacentParticle =
                m_particles[y + direction.y][x + direction.x];
            if (fastRand() % 10 > 8) {
                if ((adjacentParticle.m_attributes & Attributes::FLAMMABLE) ==
                    Attributes::FLAMMABLE) {
                    adjacentParticle.m_element = Element::FIRE;
                    adjacentParticle.m_attributes &= Attributes::FLAMMABLE;
                    adjacentParticle.m_asleep = false;
                } else if (adjacentParticle.m_element == Element::AIR) {
                    adjacentParticle = ParticleFactory::makeType(Element::FIRE);
                    adjacentParticle.m_health = 2.0F;
                }
            }
        }
    }

    // Check if target has room to spawn stuff
    const auto& direction = directions[fastRand() % 8];
    if (x + direction.x < 0 || y + direction.y < 0)
        return;
    Particle& adjacentParticle = m_particles[y + direction.y][x + direction.x];
    if (adjacentParticle.m_element != Element::AIR)
        return;

    // 2/10 chance of making smoke
    if (fastRand() % 1000 >= 925) {
        adjacentParticle = ParticleFactory::makeType(Element::SMOKE);
    }

    // 2/10 chance of creating an ember
    if (fastRand() % 1000 >= 825) {
        adjacentParticle = ParticleFactory::makeType(Element::FIRE);
    }
}

//////////////////////////////////////////////////////////////////////

void Physics::simulateElement_Smoke(const int& x, const int& y) noexcept {
    // Tick down its health
    auto& particle = m_particles[y][x];
    // particle.m_health -= 5.0F;
    particle.m_density -= 1.0F;
}

//////////////////////////////////////////////////////////////////////
/// swapTile
//////////////////////////////////////////////////////////////////////

void Physics::swapTile(Particle& particleA, Particle& particleB) noexcept {
    std::swap(particleA, particleB);
}
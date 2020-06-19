#include "physics.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <array>

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

Physics::Physics(std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>& particles)
    : m_particles(particles) {}

//////////////////////////////////////////////////////////////////////
/// findCollisions
//////////////////////////////////////////////////////////////////////

void Physics::simulate(const double& /*deltaTime*/) {
    // Apply Gravity
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            auto& particle = m_particles[y][x];

            // Only act on particles that can move
            if (!particle.m_exists || particle.m_asleep || !particle.m_moveable)
                continue;

            switch (particle.m_state) {
            case Particle::MatterState::SOLID:
                simulateSolid(x, y);
                break;
            case Particle::MatterState::LIQUID:
                simulateLiquid(x, y);
                break;
            case Particle::MatterState::GAS:
                simulateGas(x, y);
                break;
            }
        }
    }
}

void Physics::simulateSolid(const int& x, const int& y) noexcept {
    struct ivec2 {
        int x, y;
    };
    constexpr std::array<std::array<ivec2, 3>, 2> offsets = {
        std::array<ivec2, 3>{ ivec2{ 0, -1 }, { -1, -1 }, { 1, -1 } },
        std::array<ivec2, 3>{ ivec2{ 0, -1 }, { 1, -1 }, { -1, -1 } }
    };
    const auto directions = offsets[fastRand() % 2];

    auto& particle = m_particles[y][x];
    for (const auto& offset : directions) {
        if (x + offset.x < 0 || y + offset.y < 0)
            continue;

        auto& targetParticle = m_particles[y + offset.y][x + offset.x];
        if (targetParticle.m_exists &&
            (!targetParticle.m_moveable ||
             targetParticle.m_density >= particle.m_density))
            continue;

        swapTile(particle, targetParticle);
        break;
    }
}

void Physics::simulateLiquid(const int& x, const int& y) noexcept {
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
            ivec2{ 0, -1 }, { 1, -1 }, { -1, -1 }, { -1, 0 }, { 1, 0 } }
    };
    const auto directions = offsets[fastRand() % 4];

    auto& particle = m_particles[y][x];
    for (const auto& offset : directions) {
        if (x + offset.x < 0 || y + offset.y < 0)
            continue;

        auto& targetParticle = m_particles[y + offset.y][x + offset.x];
        if (targetParticle.m_exists &&
            (!targetParticle.m_moveable ||
             targetParticle.m_density >= particle.m_density))
            continue;

        swapTile(particle, targetParticle);
        break;
    }
}

void Physics::simulateGas(const int& x, const int& y) noexcept {
    struct ivec2 {
        int x, y;
    };
    constexpr std::array<std::array<ivec2, 5>, 4> offsets = {
        std::array<ivec2, 5>{
            ivec2{ 0, -1 }, { -1, 1 }, { 1, 1 }, { -1, 0 }, { 1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, -1 }, { 1, 1 }, { -1, 1 }, { 1, 0 }, { -1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, -1 }, { -1, 1 }, { 1, 1 }, { 1, 0 }, { -1, 0 } },
        std::array<ivec2, 5>{
            ivec2{ 0, -1 }, { 1, 1 }, { -1, 1 }, { -1, 0 }, { 1, 0 } }
    };
    const auto directions = offsets[fastRand() % 4];

    auto& particle = m_particles[y][x];
    for (const auto& offset : directions) {
        if (x + offset.x < 0 || y + offset.y < 0)
            continue;

        auto& targetParticle = m_particles[y + offset.y][x + offset.x];
        if (targetParticle.m_exists &&
            (!targetParticle.m_moveable ||
             targetParticle.m_density >= particle.m_density))
            continue;

        swapTile(particle, targetParticle);
        break;
    }
}

void Physics::swapTile(Particle& particleA, Particle& particleB) noexcept {
    std::swap(particleA, particleB);
}
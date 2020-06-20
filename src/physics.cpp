#include "physics.hpp"
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
    const double& /*deltaTime*/, const int& beginX, const int& beginY,
    const int& endX, const int& endY) noexcept {
    // Apply Gravity
    for (int y = beginY; y < endY; ++y) {
        for (int x = beginX; x < endX; ++x) {
            const auto& particle = m_particles[y][x];

            // Only act on particles that can move
            if (!particle.m_exists || particle.m_asleep || !particle.m_moveable)
                continue;

            switch (particle.m_state) {
            case MatterState::SOLID:
                simulateSolid(x, y);
                break;
            case MatterState::LIQUID:
                simulateLiquid(x, y);
                break;
            case MatterState::GAS:
                simulateGas(x, y);
                break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// simulateSolid
//////////////////////////////////////////////////////////////////////

void Physics::simulateSolid(const int& x, const int& y) noexcept {
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
        if (targetParticle.m_exists &&
            (!targetParticle.m_moveable ||
             targetParticle.m_density >= particle.m_density))
            continue;

        swapTile(particle, targetParticle);
        break;
    }
}

//////////////////////////////////////////////////////////////////////
/// simulateLiquid
//////////////////////////////////////////////////////////////////////

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
            ivec2{ 0, -1 }, { 1, -1 }, { -1, -1 }, { -1, 0 }, { 1, 0 } },
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

//////////////////////////////////////////////////////////////////////
/// simulateGas
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
/// swapTile
//////////////////////////////////////////////////////////////////////

void Physics::swapTile(Particle& particleA, Particle& particleB) noexcept {
    std::swap(particleA, particleB);
}
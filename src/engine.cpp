#include "engine.hpp"
#include "GLFW/glfw3.h"
#include "collision.hpp"
#include "collisionFinderSystem.hpp"
#include "collisionResolverSystem.hpp"
#include "components.hpp"
#include <algorithm>
#include <random>

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

Engine::Engine(const Window& window)
    : m_window(window), m_moveDetector(m_gameWorld),
      m_cleanupSystem(m_gameWorld), m_collisionCleanup(m_gameWorld) {
    // Random number generation variables
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);

    // Fill game world with sand
    for (auto x = 0; x < 4990; ++x) {
        ParticleComponent particle;
        particle.m_pos = vec2(
            randomFloats(generator) * 125.0F,
            (randomFloats(generator) * 0.5F + 0.5F) * 100.0F + 150.0F);
        particle.m_dimensions = vec2(1.0F);
        particle.m_type = PARTICLE_TYPE::SAND;
        PhysicsComponent physics;
        physics.m_velocity = vec2(
            randomFloats(generator) * 250.0F,
            (randomFloats(generator) * 0.5F + 0.5F) * -12.5F);
        physics.mass = (randomFloats(generator) * 0.5F + 0.5F) * 19.0F + 1.0F;
        physics.inv_mass = 1.0F / physics.mass;
        const auto entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &physics);
    }

    // Add concrete particles to world
    {
        ParticleComponent particle;
        particle.m_pos = vec2(0, -100);
        particle.m_type = PARTICLE_TYPE::CONCRETE;
        particle.m_dimensions = vec2(15.0F);
        PhysicsComponent mass;
        mass.mass = 0.0F;
        mass.inv_mass = 0.0F;
        auto entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.m_pos = vec2(-100, -150);
        particle.m_dimensions = vec2(60.0, 5.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.m_pos = vec2(55, -185);
        particle.m_dimensions = vec2(2.5F, 40.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.m_pos = vec2(60, -195);
        particle.m_dimensions = vec2(5.0F, 30.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.m_pos = vec2(70, -185);
        particle.m_dimensions = vec2(5.0F, 30.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.m_pos = vec2(80, -175);
        particle.m_dimensions = vec2(5.0F, 30.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.m_pos = vec2(90, -175);
        particle.m_dimensions = vec2(5.0F, 30.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.m_pos = vec2(100, -185);
        particle.m_dimensions = vec2(5.0F, 30.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        entityHandle = m_gameWorld.makeEntity(nullptr, 0);

        particle.m_pos = vec2(110, -195);
        particle.m_dimensions = vec2(5.0F, 30.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.m_pos = vec2(115, -185);
        particle.m_dimensions = vec2(2.5F, 40.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
    }
}

//////////////////////////////////////////////////////////////////////
/// tick
//////////////////////////////////////////////////////////////////////

void Engine::tick(const double& deltaTime) {
    gameTick(deltaTime);
    renderTick(deltaTime);
}

//////////////////////////////////////////////////////////////////////
/// gameTick
//////////////////////////////////////////////////////////////////////

void Engine::gameTick(const double& deltaTime) {
    constexpr double timeStep = 0.05;
    m_accumulator += deltaTime;
    while (m_accumulator >= timeStep) {
        // Run Game Systems
        m_gameWorld.updateSystem(&m_moveDetector, timeStep);
        m_gameWorld.updateSystem(&m_gravitySystem, timeStep);
        CollisionFinderSystem::findCollisions(timeStep, m_gameWorld);
        CollisionResolverSystem::resolveCollisions(timeStep, m_gameWorld);
        m_gameWorld.updateSystem(&m_cleanupSystem, timeStep);
        m_gameWorld.updateSystem(&m_collisionCleanup, timeStep);
        m_accumulator -= timeStep;
    }
}

//////////////////////////////////////////////////////////////////////
/// renderTick
//////////////////////////////////////////////////////////////////////

void Engine::renderTick(const double& deltaTime) {
    m_gameWorld.updateSystem(&m_renderSystem, deltaTime);
}
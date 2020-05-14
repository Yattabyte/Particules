#include "engine.hpp"
#include "GLFW/glfw3.h"
#include "collision.hpp"
#include "components.hpp"
#include <algorithm>
#include <random>

//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

Engine::Engine(const Window& window)
    : m_window(window), m_cleanupSystem(m_gameWorld) {
    // Random number generation variables
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);

    // Fill game world with sand
    for (auto x = 0; x < 1997; ++x) {
        ParticleComponent particle;
        particle.particle.m_pos =
            vec2{ randomFloats(generator) * 50.0F,
                  (randomFloats(generator) * 0.5F + 0.5F) * 50.0F + 50.0F };
        particle.particle.m_type = PARTICLE_TYPE::SAND;
        particle.particle.m_velocity = vec2{ randomFloats(generator) * 15.0F,
                                             randomFloats(generator) * 15.0F };
        PhysicsComponent mass;
        mass.mass = (randomFloats(generator) * 0.5F + 0.5F) * 24.0F + 1.0F;
        mass.inv_mass = 1.0F / mass.mass;
        BoundingBoxComponent bSphere;
        bSphere.extents = vec2(1.0F);
        MoveableComponent moveable;
        const auto entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &bSphere);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent(entityHandle, &moveable);
    }

    // Add concrete particles to world
    {
        ParticleComponent particle;
        particle.particle.m_pos = vec2{ 0, -100 };
        particle.particle.m_type = PARTICLE_TYPE::CONCRETE;
        BoundingBoxComponent bBox;
        bBox.extents = vec2(15.0F);
        PhysicsComponent mass;
        mass.mass = 0.0F;
        mass.inv_mass = 0.0F;
        auto entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &bBox);
        m_gameWorld.makeComponent(entityHandle, &mass);

        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        bBox.extents = vec2(30.0, 5.0F);
        particle.particle.m_pos = vec2{ -50, -50 };
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &bBox);
        m_gameWorld.makeComponent(entityHandle, &mass);

        bBox.extents = vec2(5.0F, 30.0F);
        entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        particle.particle.m_pos = vec2{ 50, -75 };
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &bBox);
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
        m_gameWorld.updateSystem(&m_gravitySystem, timeStep);
        m_collisionSolver.resolveCollisions(timeStep, m_gameWorld);
        m_gameWorld.updateSystem(&m_cleanupSystem, timeStep);

        m_accumulator -= timeStep;
    }
}

//////////////////////////////////////////////////////////////////////
/// renderTick
//////////////////////////////////////////////////////////////////////

void Engine::renderTick(const double& deltaTime) {
    m_gameWorld.updateSystem(&m_renderSystem, deltaTime);
}
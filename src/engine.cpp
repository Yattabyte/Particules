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
    : m_window(window), m_moveDetector(m_gameWorld), m_igniter(m_gameWorld),
      m_cleanupSystem(m_gameWorld), m_collisionCleanup(m_gameWorld) {
    // Random number generation variables
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);

    // Fill game world with sand
    for (auto x = 0; x < 3196; ++x) {
        ParticleComponent particle;
        particle.m_pos = vec2(
            randomFloats(generator) * 75.0F,
            (randomFloats(generator) * 0.5F + 0.5F) * 100.0F + -100.0F);
        particle.m_dimensions = vec2(1.0F);
        particle.m_color = COLOR_SAND;
        PhysicsComponent physics;
        FlammableComponent flammable;
        physics.m_velocity = vec2(
            randomFloats(generator) * 75.0F,
            (randomFloats(generator) * 0.5F + 0.5F) * 12.5F);
        physics.mass = (randomFloats(generator) * 0.5F + 0.5F) * 19.0F + 1.0F;
        physics.inv_mass = 1.0F / physics.mass;
        const auto entityHandle = m_gameWorld.makeEntity();
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &physics);
        m_gameWorld.makeComponent(entityHandle, &flammable);
    }

    {
        // Add concrete particles to world
        ParticleComponent particle;
        particle.m_pos = vec2(0, -200);
        particle.m_color = COLOR_CONCRETE;
        particle.m_dimensions = vec2(200.0F, 10.0f);
        PhysicsComponent mass;
        InnertComponent innert;
        mass.mass = 0.0F;
        mass.inv_mass = 0.0F;
        auto entityHandle = m_gameWorld.makeEntity();
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent(entityHandle, &innert);

        entityHandle = m_gameWorld.makeEntity();
        particle.m_pos = vec2(-190, -180);
        particle.m_dimensions = vec2(10.0F, 10.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent(entityHandle, &innert);

        entityHandle = m_gameWorld.makeEntity();
        particle.m_pos = vec2(190, -180);
        particle.m_dimensions = vec2(10.0F, 10.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent(entityHandle, &innert);

        // Add one fire particle falling down
        entityHandle = m_gameWorld.makeEntity();
        particle.m_pos = vec2(0, 250);
        particle.m_dimensions = vec2(1);
        particle.m_color = COLOR_FIRE;
        mass.mass = 50.0F;
        OnFireComponent fire;
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent(entityHandle, &fire);
    }
}

//////////////////////////////////////////////////////////////////////
/// tick
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

void Engine::tick(const double& deltaTime) {
    const auto start = glfwGetTime();
    gameTick(deltaTime);
    renderTick(deltaTime);
    const auto end = glfwGetTime();

    std::cout << std::to_string(end - start) << std::endl;
}

//////////////////////////////////////////////////////////////////////
/// gameTick
//////////////////////////////////////////////////////////////////////

void Engine::gameTick(const double& deltaTime) {
    constexpr double timeStep = 0.05;
    m_accumulator += deltaTime;
    while (m_accumulator >= timeStep) {
        // Run Game Systems
        m_gameWorld.updateSystem(m_moveDetector, timeStep);
        m_gameWorld.updateSystem(m_gravitySystem, timeStep);
        m_colFinder.findCollisions(timeStep, m_gameWorld);
        m_colResolver.resolveCollisions(timeStep, m_gameWorld);
        m_gameWorld.updateSystem(m_igniter, timeStep);
        m_gameWorld.updateSystem(m_cleanupSystem, timeStep);
        m_gameWorld.updateSystem(m_collisionCleanup, timeStep);
        m_accumulator -= timeStep;
    }
}

//////////////////////////////////////////////////////////////////////
/// renderTick
//////////////////////////////////////////////////////////////////////

void Engine::renderTick(const double& deltaTime) {
    m_gameWorld.updateSystem(m_renderSystem, deltaTime);
}
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
      m_burner(m_gameWorld), m_combuster(m_gameWorld, m_colFinder.m_quadTree),
      m_cleanupSystem(m_gameWorld), m_collisionCleanup(m_gameWorld) {
    // Random number generation variables
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);
    const auto randNum = [&](const float& low, const float& high) {
        return std::round(
            ((0.5F * randomFloats(generator) + 0.5F) * (high - low)) + low);
    };

    // Fill game world with sand
    for (auto x = 0; x < 2996; ++x) {
        ParticleComponent particle;
        PhysicsComponent physics;
        FlammableComponent flammable;
        ExplosiveComponent explosive;
        const auto entityHandle = m_gameWorld.makeEntity();

        constexpr float numEntityTypes = 4.0F;
        const int entType = static_cast<int>(randNum(0, 3));

        switch (entType) {
        // default:
        case 0: // Make Sand
            particle.m_health = 10.0F;
            particle.m_color = COLOR_SAND;
            physics.mass = randNum(8.0, 10);
            break;
        case 1: // Make Oil
            flammable.wickTime = 4.0F;
            particle.m_health = 4.0F;
            particle.m_color = COLOR_OIL;
            physics.mass = randNum(8.5F, 10.5F);
            m_gameWorld.makeComponent(entityHandle, &flammable);
            break;
        case 2: // Make Gunpowder
            explosive.fuseTime = 0.125F;
            flammable.wickTime = 1.5F;
            particle.m_health = 2.5F;
            particle.m_color = COLOR_GUNPOWDER;
            physics.mass = randNum(7.0F, 10.0F);
            m_gameWorld.makeComponent(entityHandle, &explosive);
            m_gameWorld.makeComponent(entityHandle, &flammable);
            break;
        case 3: // Make gasoline
            explosive.fuseTime = 0.875F;
            flammable.wickTime = 7.5F;
            particle.m_health = 7.5F;
            particle.m_color = COLOR_GASOLINE;
            physics.mass = randNum(9.0F, 11.0F);
            m_gameWorld.makeComponent(entityHandle, &explosive);
            m_gameWorld.makeComponent(entityHandle, &flammable);
            break;
        }

        particle.m_pos = vec2(
            randomFloats(generator) * 75.0F,
            (randomFloats(generator) * 0.5F + 0.5F) * 100.0F + -100.0F);
        particle.m_dimensions = vec2(1.0F);
        physics.m_velocity = vec2(
            randomFloats(generator) * 75.0F,
            (randomFloats(generator) * 0.5F + 0.5F) * 12.5F);
        physics.inv_mass = 1.0F / physics.mass;
        m_gameWorld.makeComponent(entityHandle, &physics);
        m_gameWorld.makeComponent(entityHandle, &particle);
    }

    {
        // Add concrete particles to world
        ParticleComponent particle;
        particle.m_pos = vec2(0, -200);
        particle.m_color = COLOR_CONCRETE;
        particle.m_dimensions = vec2(200.0F, 10.0f);
        particle.m_health = 1000.0F;
        PhysicsComponent mass;
        mass.mass = 0.0F;
        mass.inv_mass = 0.0F;
        auto entityHandle = m_gameWorld.makeEntity();
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent<InnertComponent>(entityHandle);

        entityHandle = m_gameWorld.makeEntity();
        particle.m_pos = vec2(-190, -180);
        particle.m_dimensions = vec2(10.0F, 10.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent<InnertComponent>(entityHandle);

        entityHandle = m_gameWorld.makeEntity();
        particle.m_pos = vec2(190, -180);
        particle.m_dimensions = vec2(10.0F, 10.0F);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent<InnertComponent>(entityHandle);

        // Add one fire particle falling down
        entityHandle = m_gameWorld.makeEntity();
        FlammableComponent flammable;
        flammable.wickTime = 15.0F;
        particle.m_health = 15.0F;
        particle.m_pos = vec2(0, 250);
        particle.m_dimensions = vec2(2);
        particle.m_color = COLOR_FIRE;
        mass.mass = 15.0F;
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent(entityHandle, &flammable);
        m_gameWorld.makeComponent<OnFireComponent>(entityHandle);
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
        m_gameWorld.updateSystem(m_burner, timeStep);
        m_gameWorld.updateSystem(m_combuster, timeStep);
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
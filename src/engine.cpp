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
    for (auto x = 0; x < 12499; ++x) {
        ParticleComponent particle;
        FlammableComponent flammable;
        ExplosiveComponent explosive;
        const auto entityHandle = m_gameWorld.makeEntity();

        constexpr float numEntityTypes = 4.0F;
        const int entType = static_cast<int>(randNum(0, 3));

        switch (0) {
        // default:
        case 0: // Make Sand
            particle.m_health = 10.0F;
            particle.m_color = COLOR_SAND;
            particle.mass = randNum(8.0, 10);
            break;
        case 1: // Make Oil
            flammable.wickTime = 4.0F;
            particle.m_health = 4.0F;
            particle.m_color = COLOR_OIL;
            particle.mass = randNum(8.5F, 10.5F);
            m_gameWorld.makeComponent(entityHandle, &flammable);
            break;
        case 2: // Make Gunpowder
            explosive.fuseTime = 0.125F;
            flammable.wickTime = 1.5F;
            particle.m_health = 2.5F;
            particle.m_color = COLOR_GUNPOWDER;
            particle.mass = randNum(7.0F, 10.0F);
            m_gameWorld.makeComponent(entityHandle, &explosive);
            m_gameWorld.makeComponent(entityHandle, &flammable);
            break;
        case 3: // Make gasoline
            explosive.fuseTime = 0.875F;
            flammable.wickTime = 7.5F;
            particle.m_health = 7.5F;
            particle.m_color = COLOR_GASOLINE;
            particle.mass = randNum(9.0F, 11.0F);
            m_gameWorld.makeComponent(entityHandle, &explosive);
            m_gameWorld.makeComponent(entityHandle, &flammable);
            break;
        }

        particle.m_pos = vec2(
            (randomFloats(generator) * 0.5F + 0.5F) * 500.0F,
            (randomFloats(generator) * 0.5F + 0.5F) * 100.0F + 400.0F);
        /*particle.m_velocity = vec2(
            randomFloats(generator) * 75.0F,
            (randomFloats(generator) * 0.5F + 0.5F) * 12.5F);*/
        particle.inv_mass = 1.0F / particle.mass;
        m_gameWorld.makeComponent(entityHandle, &particle);
    }

    {
        for (int y = 0; y < 1; ++y) {
            for (int x = 0; x < 500; ++x) {
                // Add concrete particles to world
                ParticleComponent particle;
                particle.m_pos =
                    vec2(static_cast<float>(x), static_cast<float>(y + 100));
                particle.m_color = COLOR_CONCRETE;
                particle.m_health = 1000.0F;
                particle.mass = 0.0F;
                particle.inv_mass = 0.0F;
                auto entityHandle = m_gameWorld.makeEntity();
                m_gameWorld.makeComponent(entityHandle, &particle);
                m_gameWorld.makeComponent<InnertComponent>(entityHandle);
            }
        }

        // Add one fire particle falling down
        ParticleComponent particle;
        FlammableComponent flammable;
        auto entityHandle = m_gameWorld.makeEntity();
        flammable.wickTime = 15.0F;
        particle.m_health = 15.0F;
        particle.m_pos = vec2(250, 250);
        particle.m_color = COLOR_FIRE;
        particle.mass = 15.0F;
        m_gameWorld.makeComponent(entityHandle, &particle);
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
        // m_gameWorld.updateSystem(m_gravitySystem, timeStep);
        m_colFinder.findCollisions(timeStep, m_gameWorld);
        // m_colResolver.resolveCollisions(timeStep, m_gameWorld);
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
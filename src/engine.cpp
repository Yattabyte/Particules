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
    : m_window(window),
      m_particleArray(std::shared_ptr<ParticleComponent* [513][513]>(
          new ParticleComponent*[513][513])),
      m_collision(m_gameWorld, m_particleArray),
      m_manifolds(m_gameWorld, m_particleArray),
      m_spawnerSystem(m_gameWorld, m_particleArray), m_igniter(m_gameWorld),
      m_burner(m_gameWorld), m_combuster(m_gameWorld, m_particleArray),
      m_cleanupSystem(m_gameWorld), m_collisionCleanup(m_gameWorld) {
    // Random number generation variables
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);
    const auto randNum = [&](const float& low, const float& high) {
        return std::round(
            ((0.5F * randomFloats(generator) + 0.5F) * (high - low)) + low);
    };

    /* // Fill game world with sand
     for (auto x = 0; x < 50000; ++x) {
         ParticleComponent particle;
         FlammableComponent flammable;
         ExplosiveComponent explosive;
         const auto entityHandle = m_gameWorld.makeEntity();

         constexpr float numEntityTypes = 4.0F;
         const int entType = static_cast<int>(randNum(0, 3));
         switch (entType) {
         // default:
         case 0: // Make Sand
             particle.m_health = 10.0F;
             particle.m_density = 1.0F;
             particle.m_color = COLOR_SAND;
             break;
         case 1: // Make Oil
             flammable.wickTime = 4.0F;
             particle.m_health = 4.0F;
             particle.m_density = 0.6F;
             particle.m_color = COLOR_OIL;
             m_gameWorld.makeComponent(entityHandle, &flammable);
             break;
         case 2: // Make Gunpowder
             explosive.fuseTime = 0.125F;
             flammable.wickTime = 1.5F;
             particle.m_health = 2.5F;
             particle.m_density = 0.8F;
             particle.m_color = COLOR_GUNPOWDER;
             m_gameWorld.makeComponent(entityHandle, &explosive);
             m_gameWorld.makeComponent(entityHandle, &flammable);
             break;
         case 3: // Make gasoline
             explosive.fuseTime = 0.875F;
             flammable.wickTime = 7.5F;
             particle.m_health = 7.5F;
             particle.m_density = 0.4F;
             particle.m_color = COLOR_GASOLINE;
             m_gameWorld.makeComponent(entityHandle, &explosive);
             m_gameWorld.makeComponent(entityHandle, &flammable);
             break;
         }
         particle.m_useGravity = true;
         particle.m_pos = vec2(
             (randomFloats(generator) * 0.5F + 0.5F) * 511,
             ((randomFloats(generator) * 0.5F + 0.5F) * 200) + 311);
         m_gameWorld.makeComponent(entityHandle, &particle);
     }*/

    {
        for (int x = 0; x < 512; ++x) {
            // Add concrete particles to world
            ParticleComponent particle;
            // FlammableComponent flammable;
            // flammable.wickTime = 1000.0F;
            particle.m_pos = vec2(static_cast<float>(x), 0.0F);
            particle.m_color = COLOR_CONCRETE;
            particle.m_health = 1000.0F;
            particle.m_density = 1000.0F;
            particle.m_useGravity = false;
            auto entityHandle = m_gameWorld.makeEntity();
            m_gameWorld.makeComponent(entityHandle, &particle);
            entityHandle = m_gameWorld.makeEntity();
            particle.m_pos = vec2(0, static_cast<float>(x));
            m_gameWorld.makeComponent(entityHandle, &particle);
            entityHandle = m_gameWorld.makeEntity();
            particle.m_pos = vec2(511, static_cast<float>(x));
            m_gameWorld.makeComponent(entityHandle, &particle);
        }

        ParticleComponent particle;
        particle.m_health = 1000.0F;
        particle.m_density = 1000.0F;
        particle.m_pos = vec2(256, 511);
        particle.m_color = COLOR_FIRE;
        particle.m_useGravity = false;
        auto entityHandle = m_gameWorld.makeEntity();
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent<SpawnerComponent>(entityHandle);
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
    constexpr double timeStep = 0.025;
    m_accumulator += deltaTime;
    while (m_accumulator >= timeStep) {
        // Clear particle pointer array
        std::fill(&m_particleArray[0][0], &m_particleArray[512][512], nullptr);
        // Apply physics
        m_gameWorld.updateSystem(m_collision, timeStep);

        // Apply collision manifolds
        // m_gameWorld.updateSystem(m_manifolds, timeStep);

        m_gameWorld.updateSystem(m_spawnerSystem, timeStep);

        // Ignite particles touching burning particles
        m_gameWorld.updateSystem(m_igniter, timeStep);
        // Hurt burning particles over-time
        m_gameWorld.updateSystem(m_burner, timeStep);
        // Explode burning combustible particles
        m_gameWorld.updateSystem(m_combuster, timeStep);
        // Delete dead or out-of-bounds particles
        m_gameWorld.updateSystem(m_cleanupSystem, timeStep);
        // Remove collision manifolds
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
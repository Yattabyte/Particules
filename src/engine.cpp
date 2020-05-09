#include "engine.hpp"
#include "GLFW/glfw3.h"
#include "collision.hpp"
#include "components.hpp"
#include <algorithm>
#include <random>

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

Engine::Engine(const Window& window)
    : m_window(window), m_collisionSystem(m_gameWorld) {
    // Random number generation variables
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);

    // Fill game world with sand
    for (auto x = 0; x < 256; ++x) {
        ParticleComponent particle;
        particle.particle.m_pos = vec2{ randomFloats(generator) * 32.0F,
                                        randomFloats(generator) * 32.0F };
        BoundingSphereComponent bSphere;
        bSphere.radius = 1.0F;
        MassComponent mass;
        mass.mass = (randomFloats(generator) * 0.5F + 0.5F) * 4.5F + 0.5F;
        MoveableComponent moveable;
        const auto entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &bSphere);
        m_gameWorld.makeComponent(entityHandle, &mass);
        m_gameWorld.makeComponent(entityHandle, &moveable);
    }

    // Add concrete particle to world
    {
        ParticleComponent particle;
        particle.particle.m_pos = vec2{ 0, -50 };
        BoundingBoxComponent bBox;
        bBox.extents = vec2(10.0F);
        const auto entityHandle = m_gameWorld.makeEntity(nullptr, 0);
        m_gameWorld.makeComponent(entityHandle, &particle);
        m_gameWorld.makeComponent(entityHandle, &bBox);
    }
}

void Engine::tick(const double& deltaTime) {
    gameTick(deltaTime);
    renderTick(deltaTime);
}

void Engine::gameTick(const double& deltaTime) {
    constexpr double timeStep = 0.01;
    m_accumulator += deltaTime;
    while (m_accumulator > timeStep) {
        // Run Game Systems
        m_gameWorld.updateSystem(&m_collisionSystem, timeStep);
        m_gameWorld.updateSystem(&m_gravitySystem, timeStep);

        m_accumulator -= timeStep;
    }
}

void Engine::renderTick(const double& deltaTime) {
    m_gameWorld.updateSystem(&m_renderSystem, deltaTime);
}
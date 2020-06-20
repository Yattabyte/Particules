#include "engine.hpp"
#include "GLFW/glfw3.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
//////////////////////////////////////////////////////////////////////
/// Custom Constructor
//////////////////////////////////////////////////////////////////////

Engine::Engine(const Window& window)
    : m_window(window),
      m_particles(std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>(
          new Particle[HEIGHT + 1][WIDTH + 1])),
      m_physics(m_particles), m_renderer(m_particles) {
    // Populate example starter level
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);
    const auto randNum = [&](const float& low, const float& high) {
        return std::round(
            ((0.5F * randomFloats(generator) + 0.5F) * (high - low)) + low);
    };
    for (auto n = 0; n < HEIGHT; ++n) {
        Particle wall;
        wall.m_exists = true;
        wall.m_moveable = false;
        wall.m_color = COLOR_CONCRETE;
        wall.m_health = 1000.0F;
        wall.m_density = 1000.0F;
        wall.m_state = MatterState::SOLID;

        m_particles[n][0] = wall;
        m_particles[n][WIDTH - 1] = wall;
    }
    for (auto n = 0; n < WIDTH; ++n) {
        Particle wall;
        wall.m_exists = true;
        wall.m_moveable = false;
        wall.m_color = COLOR_CONCRETE;
        wall.m_health = 1000.0F;
        wall.m_density = 1000.0F;
        wall.m_state = MatterState::SOLID;

        m_particles[0][n] = wall;
    }
    for (auto n = 0; n < 2000000; ++n) {
        // Get random coordinates
        vec2 pos = vec2(
            (randomFloats(generator) * 0.5F + 0.5F) * WIDTH - 1,
            (randomFloats(generator) * 0.5F + 0.5F) * HEIGHT - 1);
        const int x = static_cast<int>(pos.x());
        const int y = static_cast<int>(pos.y());
        Particle& particle = m_particles[y][x];
        if (particle.m_exists)
            continue;

        particle.m_exists = true;
        particle.m_moveable = true;

        // Get random attributes
        constexpr float numEntityTypes = 4.0F;
        const int entType = static_cast<int>(randNum(0, 3));
        switch (entType) {
        default:
        case 0: // Make Sand
            particle.m_health = 10.0F;
            particle.m_density = 1.0F;
            particle.m_color = COLOR_SAND;
            particle.m_state = MatterState::SOLID;
            break;
        case 1: // Make Oil
            // flammable.wickTime = 4.0F;
            particle.m_health = 4.0F;
            particle.m_density = 0.6F;
            particle.m_color = COLOR_WATER;
            particle.m_state = MatterState::LIQUID;
            break;
        case 2: // Make Gunpowder
            // explosive.fuseTime = 0.125F;
            // flammable.wickTime = 1.5F;
            particle.m_health = 2.5F;
            particle.m_density = 0.8F;
            particle.m_color = COLOR_GUNPOWDER;
            particle.m_state = MatterState::SOLID;
            break;
        case 3: // Make gasoline
            // explosive.fuseTime = 0.875F;
            // flammable.wickTime = 7.5F;
            particle.m_health = 7.5F;
            particle.m_density = 0.4F;
            particle.m_color = COLOR_GASOLINE;
            particle.m_state = MatterState::LIQUID;
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// tick
//////////////////////////////////////////////////////////////////////

void Engine::tick(const double& deltaTime) {
    const auto start = glfwGetTime();

    // Prepare game loop for multi-threading
    constexpr int numCellsX = WIDTH / CELL_SIZE;
    constexpr int numCellsY = HEIGHT / CELL_SIZE;
    constexpr std::pair<int, int> patternArray[8][4] = {
        { std::make_pair(0, 0), std::make_pair(1, 0), std::make_pair(1, 1),
          std::make_pair(0, 1) },
        { std::make_pair(1, 1), std::make_pair(1, 0), std::make_pair(0, 0),
          std::make_pair(0, 1) },
        { std::make_pair(0, 0), std::make_pair(0, 1), std::make_pair(1, 1),
          std::make_pair(1, 0) },
        { std::make_pair(1, 1), std::make_pair(0, 1), std::make_pair(0, 0),
          std::make_pair(1, 0) },
        { std::make_pair(0, 0), std::make_pair(1, 1), std::make_pair(0, 1),
          std::make_pair(1, 0) },
        { std::make_pair(0, 0), std::make_pair(1, 1), std::make_pair(1, 0),
          std::make_pair(0, 1) },
        { std::make_pair(1, 1), std::make_pair(0, 0), std::make_pair(1, 0),
          std::make_pair(0, 1) },
        { std::make_pair(1, 1), std::make_pair(0, 0), std::make_pair(0, 1),
          std::make_pair(1, 0) },
    };
    static int frameNum = 0;

    // Game Loop
    m_accumulator += deltaTime;
    while (m_accumulator >= TIME_STEP) {
        for (const auto& [offsetX, offsetY] : patternArray[frameNum++ % 8]) {
            for (int cellY = offsetY; cellY < numCellsY; cellY += 2) {
                const int beginY = cellY * CELL_SIZE;
                const int endY = beginY + CELL_SIZE;
                for (int cellX = offsetX; cellX < numCellsX; cellX += 2) {
                    const int beginX = cellX * CELL_SIZE;
                    const int endX = beginX + CELL_SIZE;
                    m_physics.simulate(TIME_STEP, beginX, beginY, endX, endY);
                }
            }
        }
        m_accumulator -= TIME_STEP;
    }

    // Draw
    m_renderer.draw(deltaTime);

    const auto end = glfwGetTime();
    std::cout << std::to_string(end - start) << std::endl;
}
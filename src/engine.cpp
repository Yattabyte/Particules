#include "engine.hpp"
#include "GLFW/glfw3.h"
#include "particleFactory.hpp"
#include "utilities.hpp"
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
    // Create threads
    const unsigned int maxThreads =
        std::max(1U, std::thread::hardware_concurrency() - 1);
    m_threads.resize(maxThreads);
    for (auto& [workerThread, exitSignal] : m_threads) {
        workerThread = std::thread(
            &Engine::gameTick_threaded, this, exitSignal.get_future());
        workerThread.detach();
    }

    // Populate example starter level
    std::uniform_real_distribution<float> randomFloats(-1.0F, 1.0F);
    std::mt19937 generator(0);
    const auto randNum = [&](const float& low, const float& high) {
        return std::round(
            ((0.5F * randomFloats(generator) + 0.5F) * (high - low)) + low);
    };
    for (auto n = 0; n < HEIGHT; ++n) {
        m_particles[n][0] = ParticleFactory::makeType(Element::CONCRETE);
        m_particles[n][WIDTH - 1] =
            ParticleFactory::makeType(Element::CONCRETE);
    }
    for (auto n = 0; n < WIDTH; ++n) {
        m_particles[0][n] = ParticleFactory::makeType(Element::CONCRETE);
        m_particles[HEIGHT - 1][n] =
            ParticleFactory::makeType(Element::CONCRETE);
    }

    ///*for (int n = 0; n < 512; ++n) {
    //    m_particles[1][WIDTH / 2 - (256) + n] =
    //        ParticleFactory::makeType(Element::FIRE);
    //}*/

    // for (auto n = 0; n < 1000000; ++n) {
    //    // for (auto n = 0; n < 1000; ++n) {
    //    // Get random coordinates
    //    vec2 pos = vec2(
    //        (randomFloats(generator) * 0.5F + 0.5F) * WIDTH - 1,
    //        (randomFloats(generator) * 0.5F + 0.5F) * HEIGHT - 1);
    //    const int x = static_cast<int>(pos.x());
    //    const int y = static_cast<int>(pos.y());
    //    Particle& particle = m_particles[y][x];
    //    if (particle.m_element != Element::AIR)
    //        continue;

    //    particle.m_moveable = true;

    //    // Get random attributes
    //    constexpr float numEntityTypes = 4.0F;
    //    const auto rand = fastRand();
    //    if (rand % 10 >= 8) {
    //        particle = ParticleFactory::makeType(Element::SAND);
    //    } else if (rand % 100 >= 80) {
    //        particle = ParticleFactory::makeType(Element::GASOLINE);
    //    } else if (rand % 10000 >= 9900) {
    //        particle = ParticleFactory::makeType(Element::FIRE);
    //    }
    //    /*case 3: // Make Gunpowder
    //        particle = ParticleFactory::makeType(Element::GUNPOWDER);
    //        break;
    //    case 4: // Make gasoline
    //        particle = ParticleFactory::makeType(Element::GASOLINE);
    //        break;*/
    //}
}

//////////////////////////////////////////////////////////////////////
/// tick
//////////////////////////////////////////////////////////////////////

void Engine::tick(const double& deltaTime) {
    const auto start = glfwGetTime();
    inputTick(deltaTime);
    gameTick(deltaTime);
    renderTick(deltaTime);
    const auto end = glfwGetTime();
    std::cout << std::to_string(end - start) << std::endl;
}

//////////////////////////////////////////////////////////////////////

void Engine::inputTick(const double& /*deltaTime*/) {
    if ((m_mouseEvent.m_action & MouseEvent::Action::PRESS) ==
        MouseEvent::Action::PRESS) {
        const int mouseX = static_cast<int>(m_mouseEvent.m_xPos);
        const int mouseY = HEIGHT - static_cast<int>(m_mouseEvent.m_yPos);

        // Offset within a radius to create a wider brush-stroke
        constexpr int radius = 25;
        for (int n = 0; n < radius; ++n) {
            const int x =
                std::clamp<int>((fastRand() % radius) + mouseX, 0, WIDTH);
            const int y =
                std::clamp<int>((fastRand() % radius) + mouseY, 0, HEIGHT);
            Particle& particle = m_particles[y][x];

            if (m_mouseEvent.m_button == MouseEvent::Key::LEFT) {
                particle = ParticleFactory::makeType(Element::OIL);
            } else if (m_mouseEvent.m_button == MouseEvent::Key::RIGHT) {
                particle = ParticleFactory::makeType(Element::FIRE);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Engine::gameTick(const double& deltaTime) {
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

    // Game Loop
    static int patternNum = 0;
    m_accumulator += deltaTime;
    while (m_accumulator >= TIME_STEP) {
        // Assign jobs
        for (const auto& [offsetX, offsetY] : patternArray[patternNum++ % 8]) {
            m_tickNum++;
            for (int cellY = offsetY; cellY < numCellsY; cellY += 2) {
                const int beginY = cellY * CELL_SIZE;
                const int endY = beginY + CELL_SIZE;
                for (int cellX = offsetX; cellX < numCellsX; cellX += 2) {
                    const int beginX = cellX * CELL_SIZE;
                    const int endX = beginX + CELL_SIZE;
                    m_jobs.emplace_back(beginX, beginY, endX, endY);
                }
            }
            m_numJobsRemaining = static_cast<int>(m_jobs.size());
            m_threadReady = true;
            while (m_numJobsRemaining > 0) {
                continue;
            }
            m_threadReady = false;
        }
        m_accumulator -= TIME_STEP;
    }
}

//////////////////////////////////////////////////////////////////////

void Engine::gameTick_threaded(std::future<void> exitObject) {
    while (true) {
        if (m_threadReady == false) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            continue;
        }
        // Get last job
        std::unique_lock<std::shared_mutex> writeGuard(m_jobMutex);
        if (!m_jobs.empty()) {
            const CellChunk chunk = m_jobs.back();
            m_jobs.pop_back();
            writeGuard.unlock();
            writeGuard.release();

            // Perform job
            m_physics.simulate(
                TIME_STEP, m_tickNum, chunk.m_beginX, chunk.m_beginY,
                chunk.m_endX, chunk.m_endY);
            --m_numJobsRemaining;
        } else {
            m_threadReady = false;
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Engine::renderTick(const double& deltaTime) {
    // Draw the particles
    m_renderer.draw(deltaTime);
}

//////////////////////////////////////////////////////////////////////
/// setMouseEvent
//////////////////////////////////////////////////////////////////////

void Engine::setMouseEvent(const MouseEvent& mouseEvent) {
    m_mouseEvent = mouseEvent;
}
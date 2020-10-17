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
    for (int y = 0; y < HEIGHT + 1; ++y)
        for (int x = 0; x < WIDTH + 1; ++x)
            m_physics.spawnParticle(Element::AIR, ivec2(x, y));
    for (auto n = 0; n < HEIGHT; ++n) {
        m_physics.spawnParticle(Element::CONCRETE, ivec2(0, n));
        m_physics.spawnParticle(Element::CONCRETE, ivec2(WIDTH - 1, n));
    }
    for (auto n = 0; n < WIDTH; ++n) {
        m_physics.spawnParticle(Element::CONCRETE, ivec2(n, 0));
        m_physics.spawnParticle(Element::CONCRETE, ivec2(n, HEIGHT - 1));
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

void Engine::tick(const double deltaTime) {
    const auto start = glfwGetTime();
    gameTick(deltaTime);
    renderTick(deltaTime);
    const auto end = glfwGetTime();
    std::cout << std::to_string(end - start) << std::endl;
}

//////////////////////////////////////////////////////////////////////

void Engine::inputTick(const double /*deltaTime*/) {
    // Update mouse and keyboard events
    glfwPollEvents();

    if ((m_mouseEvent.m_action & MouseEvent::Action::PRESS) ==
        MouseEvent::Action::PRESS) {
        const int mouseX = static_cast<int>(m_mouseEvent.m_xPos);
        const int mouseY = HEIGHT - static_cast<int>(m_mouseEvent.m_yPos);

        constexpr int radius = 25;
        /*// Offset within a radius to create a wider brush-stroke
            for (int n = 0; n < radius; ++n) {
                const int x =
                    std::clamp<int>((fastRand() % radius) + mouseX, 0, WIDTH);
                const int y =
                    std::clamp<int>((fastRand() % radius) + mouseY, 0, HEIGHT);

                if (x < 0 || x > WIDTH || y < 0 || y > HEIGHT)
                    continue;

                if (m_mouseEvent.m_button == MouseEvent::Key::LEFT)
                    m_physics.spawnParticle(Element::ICE, x, y);

                else if (m_mouseEvent.m_button == MouseEvent::Key::MIDDLE)
                    m_physics.spawnParticle(Element::STEAM, x, y);

                else if (m_mouseEvent.m_button == MouseEvent::Key::RIGHT)
                    m_physics.spawnParticle(Element::FIRE, x, y);
            }*/

        for (int ry = 0; ry < radius; ++ry) {
            for (int rx = 0; rx < radius; ++rx) {
                const ivec2 coords(rx + mouseX, ry + mouseY);

                if (coords.x() <= 1 || coords.x() >= WIDTH - 2 || coords.y() <= 1 || coords.y() >= HEIGHT - 2)
                    continue;

                if (m_physics.getParticle(coords).m_element != Element::AIR)
                    continue;

                if (m_mouseEvent.m_button == MouseEvent::Key::LEFT)
                    m_physics.spawnParticle(Element::ICE, coords);

                else if (m_mouseEvent.m_button == MouseEvent::Key::MIDDLE)
                    m_physics.spawnParticle(Element::WATER, coords);

                else if (m_mouseEvent.m_button == MouseEvent::Key::RIGHT)
                    m_physics.spawnParticle(Element::FIRE, coords);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Engine::gameTick(const double deltaTime) {
    // Prepare game loop for multi-threading
    constexpr int numCellsX = WIDTH / CELL_SIZE;
    constexpr int numCellsY = HEIGHT / CELL_SIZE;
    constexpr ivec2 patternArray[8][4] = {
        { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } },
        { { 1, 1 }, { 1, 0 }, { 0, 0 }, { 0, 1 } },
        { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 } },
        { { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 } },
        { { 0, 0 }, { 1, 1 }, { 0, 1 }, { 1, 0 } },
        { { 0, 0 }, { 1, 1 }, { 1, 0 }, { 0, 1 } },
        { { 1, 1 }, { 0, 0 }, { 1, 0 }, { 0, 1 } },
        { { 1, 1 }, { 0, 0 }, { 0, 1 }, { 1, 0 } },
    };

    // Game Loop
    m_threadStatus.setStatus(ThreadStatus::Status::WAITING_ON_JOBS);
    static int patternNum = 0;
    m_gameAccumulator += deltaTime;
    while (m_gameAccumulator >= TIME_STEP) {
        m_gameAccumulator -= TIME_STEP;
        inputTick(deltaTime);

        // Assign physics jobs
        for (const auto& offset : patternArray[patternNum++ % 8]) {
            m_tickNum++;
            for (int cellY = offset.y(); cellY < numCellsY; cellY += 2) {
                const int beginY = cellY * CELL_SIZE;
                const int endY = beginY + CELL_SIZE;
                for (int cellX = offset.x(); cellX < numCellsX; cellX += 2) {
                    const int beginX = cellX * CELL_SIZE;
                    const int endX = beginX + CELL_SIZE;
                    m_jobs.emplace_back(beginX, beginY, endX, endY);
                }
            }
            m_numJobsRemaining = static_cast<int>(m_jobs.size());
            m_threadStatus.setStatus(ThreadStatus::Status::READY);
            while (m_numJobsRemaining > 0) {
                continue;
            }
        }
    }
    m_threadStatus.setStatus(ThreadStatus::Status::WAITING_ON_NEXT_FRAME);
}

//////////////////////////////////////////////////////////////////////

void Engine::gameTick_threaded(std::future<void> exitObject) {
    while (true) {
        switch (m_threadStatus.getStatus()) {
        default:
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            [[fallthrough]];
        case ThreadStatus::Status::WAITING_ON_NEXT_FRAME:
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            [[fallthrough]];
        case ThreadStatus::Status::WAITING_ON_JOBS:
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            continue;
        case ThreadStatus::Status::READY: {
            // Get last job
            std::unique_lock<std::shared_mutex> writeGuard(m_jobMutex);
            if (m_jobs.empty()) {
                writeGuard.unlock();
                writeGuard.release();
                m_threadStatus.setStatus(ThreadStatus::Status::WAITING_ON_JOBS);
            } else {
                const CellChunk chunk = m_jobs.back();
                m_jobs.pop_back();
                writeGuard.unlock();
                writeGuard.release();

                // Perform job
                m_physics.simulate(m_tickNum, chunk.m_begin, chunk.m_end);
                --m_numJobsRemaining;
            }
        }
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Engine::renderTick(const double deltaTime) {
    // Draw the particles
    m_renderer.draw(deltaTime);
}

//////////////////////////////////////////////////////////////////////
/// setMouseEvent
//////////////////////////////////////////////////////////////////////

void Engine::setMouseEvent(const MouseEvent& mouseEvent) {
    m_mouseEvent = mouseEvent;
}
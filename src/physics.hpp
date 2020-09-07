#pragma once
#ifndef Physics_HPP
#define Physics_HPP

#include "definitions.hpp"
#include <memory>

///////////////////////////////////////////////////////////////////////////
/// Use the shared mini namespace
using namespace mini;

///////////////////////////////////////////////////////////////////////////
/// \class  Physics
/// \brief  Class is used to apply physics.
class Physics {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Construct a physics simulator.
    /// \param  particles       structure identifying particles spatially.
    explicit Physics(
        std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>& particles) noexcept;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Simulate 1 physics tick
    void simulate(const int tickNum, const ivec2& begin, const ivec2& end) noexcept;

    void spawnParticle(const Element elementType, const ivec2& coords);

    [[nodiscard]] Particle& getParticle(const ivec2& coords);

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Simulate state properties for particles.
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	x	            the x coordinate of the particle's tile.
    /// \param	y	            the y coordinate of the particle's tile.
    [[nodiscard]] ivec2 simulateState(const ivec2& coords) noexcept;
    [[nodiscard]] ivec2 simulateState_Solid(const ivec2& coords) noexcept;
    [[nodiscard]] ivec2 simulateState_Liquid(const ivec2& coords) noexcept;
    [[nodiscard]] ivec2 simulateState_Gas(const ivec2& coords) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Simulate element properties for particles.
    /// \param	x	            the x coordinate of the particle's tile.
    /// \param	y	            the y coordinate of the particle's tile.
    void simulateElement(const Element element, const ivec2& coords) noexcept;
    void simulateElement_Air(const ivec2& coords) noexcept;
    void simulateElement_Sand(const ivec2& coords) noexcept;
    void simulateElement_Concrete(const ivec2& coords) noexcept;
    void simulateElement_Fire(const ivec2& coords) noexcept;
    void simulateElement_Smoke(const ivec2& coords) noexcept;
    void simulateElement_Water(const ivec2& coords) noexcept;
    void simulateElement_Ice(const ivec2& coords) noexcept;
    void simulateElement_Steam(const ivec2& coords) noexcept;

    void simulateAttributes(const ivec2& coords) noexcept;

    void simulateHeat(const ivec2& coords) noexcept;

    void swapTile(const ivec2& coordA, const ivec2& coordB) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>&
        m_particles; ///< Array of particles
    std::shared_ptr<float[HEIGHT + 1][WIDTH + 1]> m_previousTemps;
};

#endif // Physics_HPP
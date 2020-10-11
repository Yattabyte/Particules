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
    void spawnParticleKeepTemp(const Element elementType, const ivec2& coords);

    [[nodiscard]] Particle& getParticle(const ivec2& coords);

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Simulate state properties for particles.
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	x	            the x coordinate of the particle's tile.
    /// \param	y	            the y coordinate of the particle's tile.
    [[nodiscard]] void simulateState(ivec2& coords) noexcept;
    [[nodiscard]] bool simulateState_Solid(ivec2& coords) noexcept;
    [[nodiscard]] bool simulateState_Liquid(ivec2& coords) noexcept;
    [[nodiscard]] bool simulateState_Gas(ivec2& coords) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Simulate element properties for particles.
    /// \param	x	            the x coordinate of the particle's tile.
    /// \param	y	            the y coordinate of the particle's tile.
    void simulateElement(const Element element, const ivec2& coords) noexcept;
    void simulateElement_AIR(const ivec2& coords) noexcept;
    void simulateElement_SAND(const ivec2& coords) noexcept;
    void simulateElement_SAWDUST(const ivec2& coords) noexcept;
    void simulateElement_CONCRETE(const ivec2& coords) noexcept;
    void simulateElement_FIRE(const ivec2& coords) noexcept;
    void simulateElement_SMOKE(const ivec2& coords) noexcept;
    void simulateElement_WATER(const ivec2& coords) noexcept;
    void simulateElement_SNOW(const ivec2& coords) noexcept;
    void simulateElement_ICE(const ivec2& coords) noexcept;
    void simulateElement_STEAM(const ivec2& coords) noexcept;
    void simulateElement_OIL(const ivec2& coords) noexcept;
    void simulateElement_GUNPOWDER(const ivec2& coords) noexcept;
    void simulateElement_GASOLINE(const ivec2& coords) noexcept;
    void simulateElement_METAL(const ivec2& coords) noexcept;

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
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
    /// \brief  Tick this system by deltaTime.
    /// \param	deltaTime	    the amount of time passed since last update.
    /// \param	components	    the components to update.
    void simulate(
        const double& deltaTime, const int& tickNum, const int& beginX,
        const int& beginY, const int& endX, const int& endY) noexcept;

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Simulate state properties for particles.
    /// \param	x	            the x coordinate of the particle's tile.
    /// \param	y	            the y coordinate of the particle's tile.
    void simulateState_Solid(const int& x, const int& y) noexcept;
    void simulateState_Liquid(const int& x, const int& y) noexcept;
    void simulateState_Gas(const int& x, const int& y) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief  Simulate element properties for particles.
    /// \param	x	            the x coordinate of the particle's tile.
    /// \param	y	            the y coordinate of the particle's tile.
    void simulateElement_Air(const int& x, const int& y) noexcept;
    void simulateElement_Sand(const int& x, const int& y) noexcept;
    void simulateElement_Concrete(const int& x, const int& y) noexcept;
    void simulateElement_Fire(const int& x, const int& y) noexcept;
    void simulateElement_Smoke(const int& x, const int& y) noexcept;

    static void swapTile(Particle& a, Particle& b) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>&
        m_particles; ///< Array of particles
};

#endif // Physics_HPP
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
        const double& deltaTime, const int& beginX, const int& beginY,
        const int& endX, const int& endY) noexcept;

    private:
    void simulateSolid(const int& x, const int& y) noexcept;
    void simulateLiquid(const int& x, const int& y) noexcept;
    void simulateGas(const int& x, const int& y) noexcept;
    static void swapTile(Particle& a, Particle& b) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    std::shared_ptr<Particle[HEIGHT + 1][WIDTH + 1]>&
        m_particles; ///< Array of particles
};

#endif // Physics_HPP
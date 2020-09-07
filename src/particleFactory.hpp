#pragma once
#ifndef PARTICLEFACTORY_HPP
#define PARTICLEFACTORY_HPP

#include "definitions.hpp"
#include <map>

/////////////////////////////////////////////////////////////////////////
/// \class  ParticleFactory
/// \brief  Makes specific particle types.
class ParticleFactory {
    public:
    ///////////////////////////////////////////////////////////////////////////
    /// Public (De)Constructors
    ~ParticleFactory() = default;
    ParticleFactory();

    ///////////////////////////////////////////////////////////////////////////
    /// Public Methods
    static ParticleFactory& getInstance();
    static Particle makeType(const Element& element);

    private:
    ///////////////////////////////////////////////////////////////////////////
    /// Private Members
    std::map<Element, Particle> m_particleTypes;
};

#endif // PARTICLEFACTORY_HPP
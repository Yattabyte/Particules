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
    ~ParticleFactory() = default;
    ParticleFactory();

    static ParticleFactory& getInstance();
    static Particle makeType(const Element& element);

    private:
    std::map<Element, Particle> m_particleTypes;
};

#endif // PARTICLEFACTORY_HPP
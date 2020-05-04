#pragma once
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "ecsComponent.hpp"
#include "particle.hpp"

struct ParticleComponent final : public ecsComponent<ParticleComponent> {
    Game_Particle particle;
};

struct MassComponent final : public ecsComponent<MassComponent> {
    float mass = 0.0F;
};

struct BoundingSphereComponent final
    : public ecsComponent<BoundingSphereComponent> {
    float radius = 1.0F;
};

struct BoundingBoxComponent final : public ecsComponent<BoundingBoxComponent> {
    vec2 extents;
};

struct MoveableComponent final : public ecsComponent<MoveableComponent> {};

#endif // COMPONENTS_HPP
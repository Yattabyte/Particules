#pragma once
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "ecsComponent.hpp"
#include "particle.hpp"

struct ParticleComponent final : public mini::ecsComponent<ParticleComponent> {
    Game_Particle particle;
};

struct MassComponent final : public mini::ecsComponent<MassComponent> {
    float mass = 0.0F;
};

struct BoundingSphereComponent final
    : public mini::ecsComponent<BoundingSphereComponent> {
    float radius = 1.0F;
};

struct BoundingBoxComponent final
    : public mini::ecsComponent<BoundingBoxComponent> {
    vec2 extents;
};

struct MoveableComponent final : public mini::ecsComponent<MoveableComponent> {
};

#endif // COMPONENTS_HPP
//* This file is part of FENIX: Fusion Energy Integrated Multiphys-X,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/fenix
//*
//* FENIX is powered by the MOOSE Fr amework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "InitializerBase.h"

class ParticlesPerElementInitializer : public InitializerBase
{
public:
  ParticlesPerElementInitializer(const InputParameters & parameters);

  static InputParameters validParams();
  /**
   * Particle initializer for uniform densities
   * the particle weight will be based on a constant charge density and
   * the specified number of particles per element
   */
  virtual std::vector<InitialParticleData> getParticleData() const override;

protected:
  /// The uniform charge density that the particle weights will be based off
  const Real _number_density;
  /// the number of particles that will be placed in each element
  const unsigned int _particles_per_element;
};

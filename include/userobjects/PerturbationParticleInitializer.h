//* This file is part of FENIX: Fusion ENergy Integrated multiphys-X,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/fenix
//* https://mooseframework.inl.gov/fenix
//*
//* FENIX is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
//* Copyright 2024, Battelle Energy Alliance, LLC
//* ALL RIGHTS RESERVED
//*

#pragma once

#include "PerElementParticleInitializer.h"

class PerturbationParticleInitializer : public PerElementParticleInitializer
{
public:
  PerturbationParticleInitializer(const InputParameters & parameters);

  static InputParameters validParams();

  virtual std::vector<InitialParticleData> getParticleData() const override;

protected: 
  /// the small shift which gives the system the sinusoidal density
  const Real _pertabtion_factor;
  const unsigned int _instability_mode;
};

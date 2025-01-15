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

#include "ElectrostaticLeapFrogStepper.h"
#include "VariableGradientSampler.h"

registerMooseObject("FenixApp", ElectrostaticLeapFrogStepper);

InputParameters
ElectrostaticLeapFrogStepper::validParams()
{
  auto params = ParticleStepperBase::validParams();
  params.addClassDescription("Particle Stepper which implements a simple leap frog update where "
                             "the velocity and position are updated with a 1/2 dt offset.");
  params.addRequiredParam<VariableName>(
      "potential",
      "The electrostatic potential from which the field will be sampled");
  return params;
}

ElectrostaticLeapFrogStepper::ElectrostaticLeapFrogStepper(const InputParameters & parameters)
  : ParticleStepperBase(parameters),
    _field_sampler(FENIX::VariableGradientSampler(_fe_problem, getParam<VariableName>("potential"), _tid))
{
}

void
ElectrostaticLeapFrogStepper::setupStep(Ray & ray, Point & v, const Real q_m_ratio, const Real distance) const
{

  // if it is the particles first step we need to take a half step
  // for the velocity other wise we take a full step
  Point F = - _field_sampler.sampleGradient(ray.currentPoint(), ray.currentElem());
  const auto dt = distance == 0 ? _dt / 2 : _dt;

  v = linearImpulse(v, F, q_m_ratio, dt);
  setMaxDistanceAndDirection(ray, v, _dt);
}

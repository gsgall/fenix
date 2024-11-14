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

#include "PeriodicChargeDensityAccumulator.h"
#include "ResidualAccumulator.h"
#include "PICStudyBase.h"
#include "MooseMesh.h"

registerMooseObject("FenixApp", PeriodicChargeDensityAccumulator);

InputParameters
PeriodicChargeDensityAccumulator::validParams()
{
  auto params = ParticleQuantityResidualAccumulatorBase::validParams();
  params.addClassDescription("Accumulator used to evaluate the inner product of the particle "
                             "charge density and the test function "
                             "required for solving electromagnetic equations.");
  return params;
}

PeriodicChargeDensityAccumulator::PeriodicChargeDensityAccumulator(const InputParameters & params)
  : ParticleQuantityResidualAccumulatorBase(params)
{
}

void
PeriodicChargeDensityAccumulator::execute()
{
  if (_fe_problem.currentlyComputingResidual())
  {
    std::unique_ptr<FENIX::AccumulatorBase> accumulator =
        std::make_unique<FENIX::ResidualAccumulator>(_fe_problem, this, _var_name, 0);

    const auto & particles = _study.getPeriodicRays(); 


    for (auto & p : particles)
    {
      if (p.elem == nullptr) 
        continue;

      accumulator->add(
          *p.elem, p.position, p.charge * p.weight);
    }

    accumulator->finalize();
  }
}

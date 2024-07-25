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

#include "SurfaceChargeRayBC.h"
#include "SurfaceChargeDensityAccumulator.h"
#include "ResidualAccumulator.h"
#include "MooseMesh.h"
#include "PICStudyBase.h"

registerMooseObject("FenixApp", SurfaceChargeDensityAccumulator);

InputParameters
SurfaceChargeDensityAccumulator::validParams()
{
  auto params = ChargeDensityAccumulatorBase::validParams();
  params.addClassDescription("Residual accumulator for contributing to the residual of a variable for particles which have contributed surface charge after hitting a boundary");
  return params;
}

SurfaceChargeDensityAccumulator::SurfaceChargeDensityAccumulator(const InputParameters & params)
  : ChargeDensityAccumulatorBase(params),
  _accumulator(std::make_unique<FENIX::ResidualAccumulator>(_fe_problem, this, _var_name, 0))
{
}

void
SurfaceChargeDensityAccumulator::initialSetup()
{
  ChargeDensityAccumulatorBase::initialSetup();
  std::vector<RayBoundaryConditionBase *> temporary_bcs;


  _study.getRayBCs(temporary_bcs, _tid);

  for (auto & bc : temporary_bcs)
  {
    auto cast = dynamic_cast<SurfaceChargeRayBC *>(bc);
    if (cast != nullptr)
      _surface_charge_bcs.push_back(cast);
  }
}

void
SurfaceChargeDensityAccumulator::execute()
{
  for (auto & bc : _surface_charge_bcs)
  {
    auto all_surface_charge_data = bc->surfaceChargeData();

    for (const auto & elemental_surface_charge_data : all_surface_charge_data)
    {
      const auto elem = elemental_surface_charge_data.first;

      for (const auto & data : elemental_surface_charge_data.second)
      {
        _accumulator->add(*elem, data.point, data.value);
      }
    }
  }
  _accumulator->finalize();
}

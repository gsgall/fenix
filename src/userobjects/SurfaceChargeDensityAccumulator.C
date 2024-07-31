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
  params.addClassDescription("Residual accumulator for contributing charge density sources to the residual of a variable for particles that were killed by a [SurfaceChargeRayBC.md].");
  return params;
}

SurfaceChargeDensityAccumulator::SurfaceChargeDensityAccumulator(const InputParameters & params)
  : ChargeDensityAccumulatorBase(params)
{
}

void
SurfaceChargeDensityAccumulator::initialSetup()
{
  ChargeDensityAccumulatorBase::initialSetup();

  _study.getRayBCs(_surface_charge_bcs, _tid);

  _surface_charge_bcs.erase(
    std::remove_if(
      _surface_charge_bcs.begin(),
      _surface_charge_bcs.end(),
      [](RayBoundaryConditionBase * bc)
      {
        return dynamic_cast<SurfaceChargeRayBC *>(bc) == nullptr;
      }
    ),
    _surface_charge_bcs.end()
  );

  if (_surface_charge_bcs.size() == 0)
    mooseError("At least one RayBC must be of type 'SurfaceChargeRayBC' for the 'SurfaceChargeAccumulator' to function properly");
}

void
SurfaceChargeDensityAccumulator::execute()
{

  if (_fe_problem.currentlyComputingResidual())
  {
    std::unique_ptr<FENIX::AccumulatorBase> accumulator =
        std::make_unique<FENIX::ResidualAccumulator>(_fe_problem, this, _var_name, 0);

    for (auto & bc : _surface_charge_bcs)
    {
      auto all_surface_charge_data = bc->surfaceChargeData();

      for (const auto & elemental_surface_charge_data : all_surface_charge_data)
      {
        const auto elem = elemental_surface_charge_data.first;
        for (const auto & data : elemental_surface_charge_data.second)
        {
          accumulator->add(*elem, data.point, data.value);
        }
      }
    }
    accumulator->finalize();
  }
}

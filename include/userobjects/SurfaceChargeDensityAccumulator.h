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
#include "ChargeDensityAccumulatorBase.h"

namespace FENIX
{
class AccumulatorBase;
}

class SurfaceChargeRayBC;
class SurfaceChargeDensityAccumulator : public ChargeDensityAccumulatorBase
{
public:
  static InputParameters validParams();

  SurfaceChargeDensityAccumulator(const InputParameters & params);

  /**
   * The execute method will get all of the particles that are being tracked
   * by the PIC study once they have finished tracing and then map the charge from
   * the particles to the residuals of a finite element variable.
   * This is computing the inner product of the point source charge density
   * and the finite element basis functions
   */
  virtual void execute() override;
  virtual void initialSetup() override;

protected:

  std::vector<SurfaceChargeRayBC *> _surface_charge_bcs;
  // std::unique_ptr<FENIX::AccumulatorBase> _accumulator;

};

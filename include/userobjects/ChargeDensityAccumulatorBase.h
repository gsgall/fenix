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
#include "GeneralUserObject.h"
// including ray for RayDataIndex
#include "Ray.h"
class PICStudyBase;

class ChargeDensityAccumulatorBase : public GeneralUserObject
{
public:
  static InputParameters validParams();

  ChargeDensityAccumulatorBase(const InputParameters & params);

  /**
   * Unused methods
   */
  ///@{
  virtual void initialize() override {}
  virtual void finalize() override {}
  ///@}

  /**
   * Actual contributions to the residual should be done in the execute method
   */
  virtual void execute() override = 0;

protected:
  /// the name of the variable which are contributing to the residual of
  const NonlinearVariableName _var_name;
  /// the PIC study that is tracking all of our particles
  const PICStudyBase & _study;
  /// the ray data index where the charge of a particle is stored
  const RayDataIndex _charge_index;
  /// the ray data index where the weight of the particle is stored
  const RayDataIndex _weight_index;
};

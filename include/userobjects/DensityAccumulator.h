//* This file is part of FENIX: Fusion Energy Integrated Multiphys-X,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/fenix
//*
//* FENIX is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once
#include "GeneralUserObject.h"
// including ray for RayDataIndex
#include "Ray.h"
class PICStudyBase;

class DensityAccumulator : public GeneralUserObject
{
public:
  static InputParameters validParams();

  DensityAccumulator(const InputParameters & params);

  /**
   * Unused methods
   */
  ///@{
  virtual void initialize() override {}
  virtual void finalize() override {}
  ///@}

  /**
   * The execute method will get all of the particles that are being tracked
   * by the PIC study once they have finished tracing and then map the charge from
   * the particles to the residuals of a finite element variable.
   * This is computing the inner product of the point source charge density
   */
  virtual void execute() override;

protected:
  /// the name of the variable which are contributing to the residual of
  const NonlinearVariableName _var_name;
  /// the PIC study that is tracking all of our particles
  const PICStudyBase & _study;
  /// the ray data index where the charge of a particle is stored
  const RayDataIndex _weight_index;
};

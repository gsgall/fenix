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

#include "PICStudyBase.h"

class ParticleStepperBase;

class EMPICStudyBase : public PICStudyBase
{
public:
  EMPICStudyBase(const InputParameters & parameters);

  static InputParameters validParams();

  /**
   * Method for getting the rays after they have finished tracing
   * useful for looking at the rays data if needed by another object
   */
  const std::vector<std::shared_ptr<Ray>> & getBankedRays() const;

  virtual void reinitializeParticles() override;

  virtual void postExecuteStudy() override;
protected:

  std::vector<InitialParticleData> _reusable_ray_dat;
  Real _prev_t;

};

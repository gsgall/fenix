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


class ParticleInitializerBase;
class TestSimpleStepper;

class TestEMPICStudy : public PICStudyBase
{
public:
  TestEMPICStudy(const InputParameters & parameters);

  static InputParameters validParams();

  /**
   * Method for getting the rays after they have finished tracing
   * useful for looking at the rays data if needed by another object
   */
  const std::vector<std::shared_ptr<Ray>> & getBankedRays() const;
  virtual void generateRays() override;
  virtual void initializeParticles() override;
  virtual void reinitializeParticles() override;

  virtual void postExecuteStudy() override;

    virtual void reinitSegment(
      const Elem * elem, const Point & start, const Point & end, const Real length, THREAD_ID tid) override;
protected:
  /// the velocity updater object which we will hold the rules for how our
  /// particles velocities are updated
  const TestSimpleStepper & _simple_stepper;
  /// the object that will supply initial data needed for rays
  const ParticleInitializerBase & _initializer;
  const bool _replicated_rays;
  Real _prev_t;
  std::vector<InitialParticleData> _reusable_ray_data;
  /// The banked rays to be used on the next timestep (restartable)
  std::vector<std::shared_ptr<Ray>> _continuing_banked_rays;
};

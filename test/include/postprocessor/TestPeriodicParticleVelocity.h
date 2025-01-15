#pragma once

#include "GeneralPostprocessor.h"
#include "PICStudyBase.h"
#include "Ray.h"

class PICStudyBase; 

class TestPeriodicParticleVelocity : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  TestPeriodicParticleVelocity(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override {}
  virtual Real getValue() const override;

protected:
  const PICStudyBase & _study; 
  const RayDataIndex _v_index; 
};

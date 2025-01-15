#pragma once

#include "GeneralPostprocessor.h"
#include "PICStudyBase.h"
#include "Ray.h"

class PICStudyBase; 

class TestPeriodicParticleEnergy : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  TestPeriodicParticleEnergy(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override {}
  virtual Real getValue() const override;

protected:
  const PICStudyBase & _study; 
  const RayDataIndex _v_index; 
  const RayDataIndex _mass_index;
  const RayDataIndex _species_index;
  const std::vector<std::string> _species; 
  std::vector<unsigned int> _species_vals; 
};

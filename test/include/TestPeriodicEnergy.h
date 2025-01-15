#pragma once 
#include "GeneralPostprocessor.h"
#include "InputParameters.h"

class TestPeriodicEnergy: GeneralPostprocessor
{
public: 
  static InputParameters validParams(); 
  TestPeriodicEnergy(const InputParameters & parameters);

  virtual void execute() override; 
  virtual void initialize() override;
  using Postprocessor::getValue;
  virtual Real getValue() const override {return 0.0;}
};

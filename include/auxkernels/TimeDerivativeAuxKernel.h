#pragma once

#include "AuxKernel.h"

class TimeDerivativeAuxKernel : public AuxKernel
{
public:
  TimeDerivativeAuxKernel(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue() override;

private:
  const VariableValue & _u;
  const VariableValue & _u_old;
};

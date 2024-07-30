#pragma once

#include "AuxKernel.h"

class TestTimeDerivativeAuxKernel : public AuxKernel
{
public:
  TestTimeDerivativeAuxKernel(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual Real computeValue() override;

private:
  /// the value of the variable you are differentiating on the current time step
  const VariableValue & _u;
  /// the value of the variable you are differentiating on the previous time step
  const VariableValue & _u_old;
};

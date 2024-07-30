#include "TestTimeDerivativeAuxKernel.h"

registerMooseObject("FenixTestApp", TestTimeDerivativeAuxKernel);

InputParameters
TestTimeDerivativeAuxKernel::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Computes the time derivative of a variable using a simple finite difference approximation");
  params.addRequiredCoupledVar("u", "The variable that will be differentiated");
  return params;
}

TestTimeDerivativeAuxKernel::TestTimeDerivativeAuxKernel(const InputParameters & parameters)
  : AuxKernel(parameters),
  _u(coupledValue("u")),
  _u_old(coupledValueOld("u"))
{
}

Real
TestTimeDerivativeAuxKernel::computeValue()
{
  return (_u[_qp] - _u_old[_qp]) / _dt;
}

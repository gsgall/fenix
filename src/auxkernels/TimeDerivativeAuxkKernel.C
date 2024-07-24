#include "TimeDerivativeAuxKernel.h"

registerMooseObject("FenixApp", TimeDerivativeAuxKernel);

InputParameters
TimeDerivativeAuxKernel::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Computes the time derivative of the aux kernel");
  params.addRequiredCoupledVar("u", "Variable we the derivative of.");
  return params;
}

TimeDerivativeAuxKernel::TimeDerivativeAuxKernel(const InputParameters & parameters)
  : AuxKernel(parameters),
  _u(coupledValue("u")),
  _u_old(coupledValueOld("u"))
{
}

Real
TimeDerivativeAuxKernel::computeValue()
{
  return (_u[_qp] - _u_old[_qp]) / _dt;
}

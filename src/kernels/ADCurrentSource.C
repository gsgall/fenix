//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADCurrentSource.h"
#include "Function.h"

registerMooseObject("FenixApp", ADCurrentSource);

InputParameters
ADCurrentSource::validParams()
{
  InputParameters params = ADVectorKernel::validParams();
  params.addClassDescription("");
  params.addRequiredParam<FunctionName>("current_density", "The vector function which holds current_source");
  return params;
}

ADCurrentSource::ADCurrentSource(const InputParameters & parameters)
  : ADVectorKernel(parameters),
    _J(getFunction("current_density"))
{
}

ADReal
ADCurrentSource::computeQpResidual()
{
  return -_test[_i][_qp] * _J.vectorValue(_t, _q_point[_qp]);
}

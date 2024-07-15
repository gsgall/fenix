//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "AmperesKernel.h"

registerMooseObject("FenixApp", AmperesKernel);

InputParameters
AmperesKernel::validParams()
{
  InputParameters params = ADVectorKernel::validParams();
  params.addClassDescription("");
  return params;
}

AmperesKernel::AmperesKernel(const InputParameters & parameters)
  : ADVectorKernel(parameters),
    _curl_test(_var.curlPhi())
{
}

ADReal
AmperesKernel::computeQpResidual()
{
  // std::cout << _u[_qp] * _curl_test[_i][_qp] << std::endl;
  // std::cout << _u[_qp] << std::endl;
  // std::cout << _curl_test[_i][_qp] << std::endl;

  return  _u[_qp] * _curl_test[_i][_qp];
}

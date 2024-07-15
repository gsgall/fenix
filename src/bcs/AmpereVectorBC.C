//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "AmpereVectorBC.h"
#include "Function.h"
#include <complex>

registerMooseObject("FenixApp", AmpereVectorBC);

InputParameters
AmpereVectorBC::validParams()
{
  InputParameters params = ADVectorIntegratedBC::validParams();
  params.addClassDescription("");
  params.addRequiredParam<FunctionName>("solution", "The vector function which holds the solution for the B field");
  return params;
}

AmpereVectorBC::AmpereVectorBC(const InputParameters & parameters)
  : ADVectorIntegratedBC(parameters),
    _B(getFunction("solution"))
{
}

ADReal
AmpereVectorBC::computeQpResidual()
{
  // std::cout << -_test[_i][_qp].cross(_B.vectorValue(_t, _q_point[_qp])) * _normals[_qp]
  //           << std::endl;
  return -_test[_i][_qp].cross(_B.vectorValue(_t, _q_point[_qp])) * _normals[_qp];
}

//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADVectorKernel.h"

class Function;

class ADCurrentSource : public ADVectorKernel
{
public:
  static InputParameters validParams();

  ADCurrentSource(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  /// the solution for the current density
  const Function & _J;

};

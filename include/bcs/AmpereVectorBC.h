//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADVectorIntegratedBC.h"

class Function;

class AmpereVectorBC : public ADVectorIntegratedBC
{
public:
  static InputParameters validParams();

  AmpereVectorBC(const InputParameters & parameters);

protected:
  ADReal computeQpResidual() override;

  /// the solution for the B field that is imposed
  const Function & _B;

};

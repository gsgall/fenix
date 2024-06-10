//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GenericRayKernel.h"

// Forward declarations
class Function;

template <bool is_ad>
class CurrentRayKernelTempl : public GenericRayKernel<is_ad>
{
public:
  CurrentRayKernelTempl(const InputParameters & params);

  static InputParameters validParams();

protected:
  virtual GenericReal<is_ad> computeQpResidual() override;

  usingGenericRayKernelMembers;
};

typedef CurrentRayKernelTempl<false> CurrentRayKernel;
typedef CurrentRayKernelTempl<true> ADCurrentRayKernel;

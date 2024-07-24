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

template <bool is_ad>
class CurrentDensityRayKernelTempl : public GenericRayKernel<is_ad>
{
public:
  CurrentDensityRayKernelTempl(const InputParameters & params);

  static InputParameters validParams();

protected:
  virtual GenericReal<is_ad> computeQpResidual() override;
  const unsigned int _component;
  const RayDataIndex _charge_index;
  const RayDataIndex _weight_index;
  RayDataIndex _velocity_index;

  usingGenericRayKernelMembers;
};

typedef CurrentDensityRayKernelTempl<false> CurrentDensityRayKernel;
typedef CurrentDensityRayKernelTempl<true> ADCurrentDensityRayKernel;

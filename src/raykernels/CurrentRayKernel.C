//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "CurrentRayKernel.h"

registerMooseObject("FenixApp", CurrentRayKernel);
registerMooseObject("FenixApp", ADCurrentRayKernel);

template <bool is_ad>
InputParameters
CurrentRayKernelTempl<is_ad>::validParams()
{
  InputParameters params = GenericRayKernel<is_ad>::validParams();

  params.addClassDescription("Ray Kernel for mapping current from the particles to the mesh");

  return params;
}

template <bool is_ad>
CurrentRayKernelTempl<is_ad>::CurrentRayKernelTempl(const InputParameters & params)
  : GenericRayKernel<is_ad>(params)
{
}

template <bool is_ad>
GenericReal<is_ad>
CurrentRayKernelTempl<is_ad>::computeQpResidual()
{
  return 0.0;
}

template class CurrentRayKernelTempl<false>;
template class CurrentRayKernelTempl<true>;

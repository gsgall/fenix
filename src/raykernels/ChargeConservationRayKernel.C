//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ChargeConservationRayKernel.h"

registerMooseObject("FenixApp", ChargeConservationRayKernel);
registerMooseObject("FenixApp", ADChargeConservationRayKernel);

template <bool is_ad>
InputParameters
ChargeConservationRayKernelTempl<is_ad>::validParams()
{
  InputParameters params = GenericRayKernel<is_ad>::validParams();

  params.addClassDescription("Ray Kernel for mapping current from the particles to the mesh");
  params.addRangeCheckedParam<unsigned int>("component", "component < 3", "The component of the current you are contributing to");
  return params;
}

template <bool is_ad>
ChargeConservationRayKernelTempl<is_ad>::ChargeConservationRayKernelTempl(const InputParameters & params)
  : GenericRayKernel<is_ad>(params),
    _charge_index(this->_study.getRayDataIndex("charge")),
    _weight_index(this->_study.getRayDataIndex("weight")),
    _grad_test(this->_var.gradPhi())
{
}

template <bool is_ad>\
GenericReal<is_ad>
ChargeConservationRayKernelTempl<is_ad>::computeQpResidual()
{
  const auto & ray = currentRay();
  return -ray->data(_charge_index) * ray->data(_weight_index) * ray->direction() * _grad_test[_i][_qp] / this->_dt;
}

template class ChargeConservationRayKernelTempl<false>;
template class ChargeConservationRayKernelTempl<true>;

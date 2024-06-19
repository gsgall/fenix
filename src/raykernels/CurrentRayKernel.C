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
  params.addRangeCheckedParam<unsigned int>("component", "component < 3", "The component of the current you are contributing to");
  return params;
}

template <bool is_ad>
CurrentRayKernelTempl<is_ad>::CurrentRayKernelTempl(const InputParameters & params)
  : GenericRayKernel<is_ad>(params),
    _component(this->template getParam<unsigned int>("component")),
    _charge_index(this->_study.getRayDataIndex("charge")),
    _weight_index(this->_study.getRayDataIndex("weight"))
{
  _velocity_index = _component == 0 ? this->_study.getRayDataIndex("v_x") :
                    _component == 1 ? this->_study.getRayDataIndex("v_y") :
                                      this->_study.getRayDataIndex("v_z");
}

template <bool is_ad>
GenericReal<is_ad>
CurrentRayKernelTempl<is_ad>::computeQpResidual()
{
  const auto & ray = currentRay();
  return -ray->data(_charge_index) * ray->data(_weight_index) * ray->data(_velocity_index) / ray->maxDistance() * _test[_i][_qp];
}

template class CurrentRayKernelTempl<false>;
template class CurrentRayKernelTempl<true>;

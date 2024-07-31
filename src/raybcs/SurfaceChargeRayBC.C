//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SurfaceChargeRayBC.h"

registerMooseObject("FenixApp", SurfaceChargeRayBC);

InputParameters
SurfaceChargeRayBC::validParams()
{
  auto params = KillRayBC::validParams();
  params.addClassDescription("[KillRayBC.md] which keeps track of where rays are killed and the point where they are killed.");
  return params;
}

SurfaceChargeRayBC::SurfaceChargeRayBC(const InputParameters & params)
  : KillRayBC(params),
    _charge_index(_study.getRayDataIndex("charge")),
    _weight_index(_study.getRayDataIndex("weight"))
{}

void
SurfaceChargeRayBC::onBoundary(const unsigned int num_applying)
{
  KillRayBC::onBoundary(num_applying);
  const auto & ray = currentRay();
  auto & data = _charge_data[ray->currentElem()].emplace_back();
  data.point = ray->currentPoint();
  data.value = ray->data(_charge_index) * ray->data(_weight_index);
}

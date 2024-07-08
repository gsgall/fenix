//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "KillParticleBC.h"

registerMooseObject("FenixApp", KillParticleBC);

InputParameters
KillParticleBC::validParams()
{
  auto params = GeneralRayBC::validParams();
  params.addClassDescription("A RayBC that kills a Ray on a boundary.");
  return params;
}

KillParticleBC::KillParticleBC(const InputParameters & params) : KillRayBC(params) {}

void
KillParticleBC::onBoundary(const unsigned int num_applying)
{
  KillRayBC::onBoundary(num_applying);

  const auto & ray = currentRay();
  if (ray->processorCrossings() == 0)
  {
    _local_dt[ray->id()] = (ray->distance() / ray->maxDistance()) * _dt;
    return;
  }

  _transmittable_local_dt[ray->id()] = (ray->distance() / ray->maxDistance()) * _dt;
}


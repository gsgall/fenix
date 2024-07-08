//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "KillRayBC.h"
/**
 * RayBC that kills a Ray
 */
class KillParticleBC : public KillRayBC
{
public:
  KillParticleBC(const InputParameters & params);

  static InputParameters validParams();

  virtual void onBoundary(const unsigned int num_applying) override;

  std::unordered_map<RayID, Real> & getLocalParticleDts() {return _local_dt;}
  std::unordered_map<RayID, Real> & getTransmittableParticleDts() {
    return _transmittable_local_dt;
  }

protected:
  std::unordered_map<RayID, Real> _local_dt;
  std::unordered_map<RayID, Real> _transmittable_local_dt;
};

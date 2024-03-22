//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TestParticleMotionData.h"
#include "PICStudyBase.h"

registerMooseObject("FenixApp", TestParticleMotionData);

InputParameters
TestParticleMotionData::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addClassDescription("Collect that value of ray data on every time step");
  params.addRequiredParam<UserObjectName>("study", "The RayTracingStudy that owns the Ray");
  return params;
}

TestParticleMotionData::TestParticleMotionData(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _study(getUserObject<PICStudyBase>("study")),
    _t_pos(declareVector("t_pos")),
    _x(declareVector("x")),
    _y(declareVector("y")),
    _z(declareVector("z")),
    _t_vel(declareVector("t_vel")),
    _v_x(declareVector("v_x")),
    _v_y(declareVector("v_y")),
    _v_z(declareVector("v_z"))
{
}

void
TestParticleMotionData::initialize()
{
}

void
TestParticleMotionData::execute()
{
  const auto rays = _study.getBankedRays();
  int count = 0;
  for (auto it = rays.begin(); rays.size() != 0 && it != rays.end(); ++it)
  {
    auto ray = *it;
    _t_pos.push_back(_t);
    _x.push_back(ray->currentPoint()(0));
    _y.push_back(ray->currentPoint()(1));
    _z.push_back(ray->currentPoint()(2));
    _t_vel.push_back(_t - _dt / 2);
    _v_x.push_back(ray->data()[_study.getRayDataIndex("v_x")]);
    _v_y.push_back(ray->data()[_study.getRayDataIndex("v_y")]);
    _v_z.push_back(ray->data()[_study.getRayDataIndex("v_z")]);
    count++;
  }
}

void
TestParticleMotionData::finalize()
{
  // collect all processor data
  comm().gather(0, _t_pos);
  comm().gather(0, _x);
  comm().gather(0, _y);
  comm().gather(0, _z);
  comm().gather(0, _t_vel);
  comm().gather(0, _v_x);
  comm().gather(0, _v_y);
  comm().gather(0, _v_z);
}

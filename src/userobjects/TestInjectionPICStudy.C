//* This file is part of FENIX: Fusion Energy Integrated Multiphys-X,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/fenix
//*
//* FENIX is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TestInjectionPICStudy.h"
#include <limits>
#include "ParticleInitializerBase.h"
#include "TestSimpleStepper.h"
#include "ClaimRays.h"

registerMooseObject("FenixApp", TestInjectionPICStudy);
InputParameters
TestInjectionPICStudy::validParams()
{
  auto params = PICStudyBase::validParams();
  params.addClassDescription("Base class for PIC studies. Provides some of the basic ray data "
                             "needed for particle tracking."
                             "Basic logic for resetting rays to be used as particles after the "
                             "original generation is complete is also provided");
  params.addRequiredParam<UserObjectName>("initializer",
                                          "The initializer that will place particles");
  return params;
}

TestInjectionPICStudy::TestInjectionPICStudy(const InputParameters & parameters)
  : PICStudyBase(parameters),
    _initializer(getUserObject<ParticleInitializerBase>("initializer"))
{
}

void
TestInjectionPICStudy::initializeParticles()
{
  auto initial_data = _initializer.getParticleData();
  // if there are no rays on this processor: do nothing
  if (initial_data.size() == 0)
    return;

  std::vector<std::shared_ptr<Ray>> rays(initial_data.size());

  for (unsigned int i = 0; i < initial_data.size(); i++)
  {
    rays[i] = acquireRay();
    rays[i]->setStart(initial_data[i].position, initial_data[i].elem);
    rays[i]->data(_v_x_index) = initial_data[i].velocity(0);
    rays[i]->data(_v_y_index) = initial_data[i].velocity(1);
    rays[i]->data(_v_z_index) = initial_data[i].velocity(2);
    rays[i]->data(_mass_index) = initial_data[i].mass;
    rays[i]->data(_weight_index) = initial_data[i].weight;
    rays[i]->data(_charge_index) = initial_data[i].charge;
    _stepper.setupStep(
        *rays[i], initial_data[i].velocity, initial_data[i].charge / initial_data[i].mass);
    setVelocity(*rays[i], initial_data[i].velocity);
  }
  moveRaysToBuffer(rays);
}

void
TestInjectionPICStudy::reinitializeParticles()
{
  for (auto & ray : _banked_rays)
  {
    // Store off the ray's info before we reset it
    const auto elem = ray->currentElem();
    const auto point = ray->currentPoint();
    const auto distance = ray->distance();

    getVelocity(*ray, _temporary_velocity);
    // Reset it (this is required to reuse a ray)
    ray->resetCounters();
    ray->clearStartingInfo();

    // And set the new starting information
    ray->setStart(point, elem);
    _stepper.setupStep(
        *ray, _temporary_velocity, ray->data()[_charge_index] / ray->data()[_mass_index], distance);

    setVelocity(*ray, _temporary_velocity);
  }

  auto additional_rays = _initializer.getParticleData();

  for (auto & data : additional_rays)
  {
    auto & ray = _banked_rays.emplace_back(acquireRay());
    ray->setStart(data.position, data.elem);
    ray->data(_v_x_index) = data.velocity(0);
    ray->data(_v_y_index) = data.velocity(1);
    ray->data(_v_z_index) = data.velocity(2);
    ray->data(_mass_index) = data.mass;
    ray->data(_weight_index) = data.weight;
    ray->data(_charge_index) = data.charge;
    _stepper.setupStep(
        *ray, data.velocity, data.charge / data.mass);
    setVelocity(*ray, data.velocity);
  }

}

void
TestInjectionPICStudy::generateRays()
{
  // We generate rays the first time only, after that we will
  // pull from the bank and update velocities/max distances
  if (!_has_generated)
  {
    initializeParticles();
    _has_generated = true;
    return;
  }

  reinitializeParticles();
  moveRaysToBuffer(_banked_rays);
  return;
}

void
TestInjectionPICStudy::postExecuteStudy()
{
  // we are going to be re using the same rays which just took a step so
  // we store them here to reset them in the generateRays method
  _banked_rays = rayBank();

  _banked_rays.erase(
    std::remove_if(
      begin(_banked_rays),
      end(_banked_rays),
      [](const std::shared_ptr<Ray> r)
      {
        return r->distance() < r->maxDistance();
      }
    ),
    end(_banked_rays)
  );
}

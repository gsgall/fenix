//* This file is part of FENIX: Fusion ENergy Integrated multiphys-X,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/fenix
//* https://mooseframework.inl.gov/fenix
//*
//* FENIX is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
//* Copyright 2024, Battelle Energy Alliance, LLC
//* ALL RIGHTS RESERVED
//*

#include "Test1DPeriodicStudy.h"
#include "ParticleStepperBase.h"
#include "ClaimRays.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>

registerMooseObject("FenixTestApp", Test1DPeriodicStudy);

InputParameters
Test1DPeriodicStudy::validParams()
{
  auto params = TestInitializedPICStudy::validParams();
  return params;
}

Test1DPeriodicStudy::Test1DPeriodicStudy(const InputParameters & parameters)
  : TestInitializedPICStudy(parameters),
  _x_min (std::numeric_limits<float>::max()),
  _x_max (std::numeric_limits<float>::lowest()), 
  _domain_length (0)
{ 
  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    _domain_length += elem->volume();
    for (const auto & node : elem->node_ref_range())
    {
      if (node(0) < _x_min)
        _x_min = node(0);

      if (node(0) > _x_max)
        _x_max = node(0);
    }
  }
  
  comm().sum(_domain_length);
  comm().min(_x_min);
  comm().max(_x_max);
}
void
Test1DPeriodicStudy::reinitializeParticles()
{
  // Reset each ray
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
  
  if (_periodic_particles.empty())
    return; 

  std::vector<std::shared_ptr<Ray>> _unclaimed_rays;
  for (const auto i : make_range(_periodic_particles.size()))
  {
    auto & ray = _unclaimed_rays.emplace_back(acquireReplicatedRay()); 

    setInitialParticleData(ray, _periodic_particles[i]); 
  }


  std::vector<std::shared_ptr<Ray>> claimed_rays;
  ClaimRays claim_rays(*this, _unclaimed_rays, claimed_rays, false);
  claim_rays.claim();
  // lets loop through the claimed rays and set them up for the step
  // we need to do so because before they are claimed the ray does not
  // know what element it is in
  for (auto & ray : claimed_rays)
  {
    getVelocity(*ray, _temporary_velocity);
    _stepper.setupStep(
        *ray, _temporary_velocity, ray->data()[_charge_index] / ray->data()[_mass_index]);
    setVelocity(*ray, _temporary_velocity);
  }
  // ...and then add them to be traced
  moveRaysToBuffer(claimed_rays);
}


void 
Test1DPeriodicStudy::postExecuteStudy() {  
  _periodic_particles.clear(); 
  _banked_rays = rayBank(); 
  
  std::vector<Real> x_pos, mass, charge, weight, vx, vy, vz; 
  std::vector<int> species; 


  _banked_rays.erase(
    std::remove_if(
      _banked_rays.begin(),
      _banked_rays.end(),
      [this, &x_pos, &mass, &charge, &weight, &species, &vx, &vy, &vz](const std::shared_ptr<Ray> & ray)
      {
        std::cout << ray->getInfo() << std::endl;
        if (std::abs(ray->distance() - ray->maxDistance()) / ray->maxDistance() < 1e-6 ||  ray->stationary())
          return false;
        
        mass.push_back(ray->data(_mass_index)); 
        charge.push_back(ray->data(_charge_index)); 
        weight.push_back(ray->data(_weight_index)); 
        species.push_back(ray->data(_species_index)); 
        vx.push_back(ray->data(_v_x_index)); 
        vy.push_back(ray->data(_v_y_index)); 
        vz.push_back(ray->data(_v_z_index)); 

        if (std::signbit(ray->direction()(0))) 
        {
          x_pos.push_back(_x_max - std::abs(std::fmod(ray->maxDistance() - ray->distance(), _domain_length)));
        }
        else 
        {
          x_pos.push_back(_x_min + std::abs(std::fmod(ray->maxDistance() - ray->distance(), _domain_length)));
        }

        return true; 
      }),
      _banked_rays.end());
      comm().allgather(x_pos, false); 
      comm().allgather(mass, false); 
      comm().allgather(charge, false); 
      comm().allgather(species, false); 
      comm().allgather(weight, false); 
      comm().allgather(vx, false); 
      comm().allgather(vy, false); 
      comm().allgather(vz, false); 

      for (const auto i : make_range(x_pos.size()))
      { 
        auto & data = _periodic_particles.emplace_back(); 
        data.elem = nullptr; 
        data.position(0) = x_pos[i]; 
        data.mass = mass[i]; 
        data.charge = charge[i]; 
        data.species = species[i]; 
        data.weight = weight[i]; 
        data.velocity(0) = vx[i]; 
        data.velocity(1) = vy[i]; 
        data.velocity(2) = vz[i]; 
      }
}

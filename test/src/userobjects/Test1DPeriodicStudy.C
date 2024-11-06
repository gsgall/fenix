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
Test1DPeriodicStudy::postExecuteStudy() {  
  _periodic_particles.clear(); 
  _banked_rays = rayBank(); 
  Point curr_pos; 
  for (const auto & ray : _banked_rays) 
  {
    if (std::abs(ray->distance() - ray->maxDistance()) / ray->maxDistance() < 1e-6)
      break;
    if (std::signbit(ray->direction()(0))) 
    {
      curr_pos(0) = _x_max - std::abs(std::remainder(ray->maxDistance() - ray->distance(), _domain_length));
    }
    else 
    {
      curr_pos(0) = _x_min + std::abs(std::remainder(ray->maxDistance() - ray->distance(), _domain_length));
    }
  }
//   // TODO: add the parallel version of this but 
//   // need to concatinate all of the data from all of the processes 
//  // and then make sure all processes have all of the data so we can create replicated rays
//  // potentially could use a gather to the root and then broadcast everything to everyone but 
//  // // need to check if there is a method like set_union for this vector of structs 
//  std::cout << _periodic_particles.size() << std::endl;

}

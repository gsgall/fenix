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

#include "ActionWarehouse.h"
#include "PerElementParticleInitializer.h"
#include "PerturbationParticleInitializer.h"
#include "MooseRandom.h"
#include "ElementSampler.h"
#include "Distribution.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <math.h>
#include <limits>

registerMooseObject("FenixApp", PerturbationParticleInitializer);

InputParameters
PerturbationParticleInitializer::validParams()
{
  auto params = PerElementParticleInitializer::validParams(); 
  params.addClassDescription("Initializer for instability studies");
  params.addRangeCheckedParam<Real>("perturbation_length",
                                    "perturbation_length > 0.0",
                                    "The perturbation factor must be positive"); 
  params.addRangeCheckedParam<unsigned int>("instability_mode", 
                                            "instability_mode > 0", 
                                            "The mode numer of the perturbation which will cause the instability");

  return params;
}

PerturbationParticleInitializer::PerturbationParticleInitializer(const InputParameters & parameters)
  : PerElementParticleInitializer(parameters), 
  _pertabtion_factor(getParam<Real>("perturbation_length")), 
  _instability_mode(getParam<unsigned int>("instability_mode"))
{
  if (_mesh_dimension != 1)
    mooseError("The simulation must be in 1D in order to use the PerturbationParticleInitializer");
}

std::vector<InitialParticleData>
PerturbationParticleInitializer::getParticleData() const
{
  Real local_xmin = std::numeric_limits<float>::max();
  Real global_xmax = std::numeric_limits<float>::lowest();
  Real local_volume = 0;
  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    local_volume += elem->volume();
    for (const auto & node : elem->node_ref_range())
    {
      if (node(0) < local_xmin)
        local_xmin = node(0);

      if (node(0) > global_xmax)
        global_xmax = node(0);
    }
  }
  Real global_volume = local_volume;
  Real global_xmin = local_xmin;

  comm().sum(global_volume);
  comm().min(global_xmin);
  comm().max(global_xmax);
  
  Real domain_length = std::abs(global_xmax - global_xmin);


  // counting the number of elements this process is responsible for
  // this will allow us to allocated data structures of the appropriate length
  const auto & elem_range = *_fe_problem.mesh().getActiveLocalElementRange();
  const auto num_local_elements = std::distance(elem_range.begin(), elem_range.end());
  auto total_elements = num_local_elements; 
  comm().sum(total_elements);
  // if there are no elements for this processor: do nothing
  if (num_local_elements == 0)
    return {};

  std::vector<InitialParticleData> data =
      std::vector<InitialParticleData>(num_local_elements * _particles_per_element);

  // random number generator to be used for sampling the elements
   MooseRandom generator;

  Real dx = domain_length / (total_elements * _particles_per_element);
  
  std::vector<Point> physical_points = std::vector<Point>(total_elements * _particles_per_element);
  for (const auto & i : make_range(physical_points.size()))
  {
    physical_points[i](0) = (i + 0.5) * dx; 
    physical_points[i](0) += _pertabtion_factor * std::sin(2 * M_PI * _instability_mode / domain_length * physical_points[i](0));

    if (physical_points[i](0) > global_xmax) 
    {
      physical_points[i](0) -= domain_length;
    } else if (physical_points[i](0) < global_xmin)
    {
      physical_points[i](0) += domain_length;
    }

  }

  std::sort(physical_points.begin(), physical_points.end());
  
  unsigned int particle_index = 0;
  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    Real weight = _number_density * elem->volume() / (_particles_per_element);
    while(particle_index < physical_points.size() && elem->contains_point(physical_points[particle_index]))
    {
      data[particle_index].elem = elem;
      data[particle_index].weight = weight;
      data[particle_index].species = _species;
      data[particle_index].mass = _mass;
      data[particle_index].charge = _charge;
      data[particle_index].position = physical_points[particle_index];
      data[particle_index].velocity = Point();
      for (const auto i : make_range(uint(3)))
        data[particle_index].velocity(i) = _velocity_distributions[i]->quantile(generator.rand());
      particle_index++;
    }
  }
  return data;
}

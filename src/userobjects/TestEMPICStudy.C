//* This file is part of FENIX: Fusion Energy Integrated Multiphys-X,
//* A multiphysics application for modeling plasma facing components
//* https://github.com/idaholab/fenix
//*
//* FENIX is powered by the MOOSE Framework
//* https://www.mooseframework.inl.gov
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TestEMPICStudy.h"
#include <limits>
#include "ParticleInitializerBase.h"
#include "TestSimpleStepper.h"
#include "ClaimRays.h"
#include "RayKernelBase.h"
#include "TraceRay.h"
#include "Ray.h"

registerMooseObject("FenixApp", TestEMPICStudy);
InputParameters
TestEMPICStudy::validParams()
{
  auto params = PICStudyBase::validParams();
  params.addClassDescription("Base class for PIC studies. Provides some of the basic ray data "
                             "needed for particle tracking."
                             "Basic logic for resetting rays to be used as particles after the "
                             "original generation is complete is also provided");
  params.addRequiredParam<UserObjectName>("initializer",
                                          "The initializer that will place particles");
  params.addRequiredParam<UserObjectName>(
      "simple_stepper",
      "the stepper for rays that will be retraced for kernel contributions");
  params.addParam<bool>("replicated_rays", true, "wether or not to have replicated rays");
  return params;
}

TestEMPICStudy::TestEMPICStudy(const InputParameters & parameters)
  : PICStudyBase(parameters),
    _simple_stepper(getUserObject<TestSimpleStepper>("simple_stepper")),
    _initializer(getUserObject<ParticleInitializerBase>("initializer")),
    _replicated_rays(getParam<bool>("replicated_rays")),
    _prev_t(std::numeric_limits<float>::lowest())
{

  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    _current_data[elem];

  }
}

void
TestEMPICStudy::initializeParticles()
{
  auto initial_data = _initializer.getParticleData();
  // if there are no rays on this processor: do nothing
  if (initial_data.size() == 0)
    return;

  std::vector<std::shared_ptr<Ray>> rays(initial_data.size());

  for (unsigned int i = 0; i < initial_data.size(); i++)
  {
    if (_replicated_rays)
    {
      rays[i] = acquireReplicatedRay();
    } else {
      rays[i] = acquireRay();
    }
    rays[i]->setStart(initial_data[i].position, initial_data[i].elem);
    rays[i]->data(_v_x_index) = initial_data[i].velocity(0);
    rays[i]->data(_v_y_index) = initial_data[i].velocity(1);
    rays[i]->data(_v_z_index) = initial_data[i].velocity(2);
    rays[i]->data(_mass_index) = initial_data[i].mass;
    rays[i]->data(_weight_index) = initial_data[i].weight;
    rays[i]->data(_charge_index) = initial_data[i].charge;
    if (!_replicated_rays)
    {
      _stepper.setupStep(
          *rays[i], initial_data[i].velocity, initial_data[i].charge / initial_data[i].mass);
      setVelocity(*rays[i], initial_data[i].velocity);
      _reusable_ray_data = initial_data;
    }
  }

  if (!_replicated_rays)
  {
    moveRaysToBuffer(rays);
    return;
  }
  // Claim the rays
  std::vector<std::shared_ptr<Ray>> claimed_rays;
  ClaimRays claim_rays(*this, rays, claimed_rays, false);
  claim_rays.claim();
  // lets loop through the claimed rays and set them up for the step
  // we need to do so because before they are claimed the ray does not
  // know what element it is in
  _reusable_ray_data = std::vector<InitialParticleData>(claimed_rays.size());
  int i = 0;
  for (auto & ray : claimed_rays)
  {
    getVelocity(*ray, _temporary_velocity);
    _stepper.setupStep(
        *ray, _temporary_velocity, ray->data()[_charge_index] / ray->data()[_mass_index]);
    setVelocity(*ray, _temporary_velocity);
    _reusable_ray_data[i].position = ray->currentPoint();
    _reusable_ray_data[i].elem = ray->currentElem();
    _reusable_ray_data[i].velocity(0)  = ray->data(_v_x_index);
    _reusable_ray_data[i].velocity(1)  = ray->data(_v_y_index);
    _reusable_ray_data[i].velocity(2)  = ray->data(_v_z_index);
    _reusable_ray_data[i].mass  = ray->data(_mass_index);
    _reusable_ray_data[i].weight  = ray->data(_weight_index);
    _reusable_ray_data[i].charge  = ray->data(_charge_index);
  }
  moveRaysToBuffer(claimed_rays);
}

void
TestEMPICStudy::reinitializeParticles()
{
  // Reset each ray
  _reusable_ray_data.resize(_banked_rays.size());
  unsigned int count = 0;
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

    // storing everything we need to retrace the ray

    _reusable_ray_data[count].elem = elem;
    _reusable_ray_data[count].position = point;
    _reusable_ray_data[count].mass = ray->data(_mass_index);
    _reusable_ray_data[count].weight = ray->data(_weight_index);
    _reusable_ray_data[count].charge = ray->data(_charge_index);
    _reusable_ray_data[count].velocity(0) = ray->data(_v_x_index);
    _reusable_ray_data[count].velocity(1) = ray->data(_v_y_index);
    _reusable_ray_data[count].velocity(2) = ray->data(_v_z_index);
    count++;
  }

  // auto additional_data = _initializer.getParticleData();
  // for (auto & data : additional_data)
  // {
  //   auto & r = _banked_rays.emplace_back(acquireRay());
  //   r->setStart(data.position, data.elem);
  //   r->data(_v_x_index) = data.velocity(0);
  //   r->data(_v_y_index) = data.velocity(1);
  //   r->data(_v_z_index) = data.velocity(2);
  //   r->data(_mass_index) = data.mass;
  //   r->data(_weight_index) = data.weight;
  //   r->data(_charge_index) = data.charge;
  //   _stepper.setupStep(
  //       *r, data.velocity, data.charge / data.mass);
  //   setVelocity(*r, data.velocity);
  // }
  // _reusable_ray_data.insert(
  //   _reusable_ray_data.end(),
  //   std::make_move_iterator(additional_data.begin()),
  //   std::make_move_iterator(additional_data.end())
  // );
  // resizing the container so that
  // we are not trying trace rays
}

void
TestEMPICStudy::generateRays()
{
  // We generate rays the first time only, after that we will
  // pull from the bank and update velocities/max distances
  if (!_has_generated)
  {
    initializeParticles();
    _has_generated = true;
    return;
  }

  if (_prev_t != _t)
  {
    reinitializeParticles();
    // Add the rays to be traced
    moveRaysToBuffer(_banked_rays);
    return;
  }

  std::vector<std::shared_ptr<Ray>> rays(_reusable_ray_data.size());
  for (unsigned int i = 0; i < _reusable_ray_data.size(); ++i)
  {
    rays[i] = acquireRay();
    rays[i]->setStart(_reusable_ray_data[i].position, _reusable_ray_data[i].elem);
    rays[i]->data(_mass_index) = _reusable_ray_data[i].mass;
    rays[i]->data(_weight_index) = _reusable_ray_data[i].weight;
    rays[i]->data(_charge_index) = _reusable_ray_data[i].charge;
    setVelocity(*rays[i], _reusable_ray_data[i].velocity);
    _simple_stepper.setupStep(*rays[i],
                              _reusable_ray_data[i].velocity,
                              rays[i]->data(_charge_index) / rays[i]->data(_mass_index));
  }
  moveRaysToBuffer(rays);
}

void
TestEMPICStudy::postExecuteStudy()
{
  // we are going to be re using the same rays which just took a step so
  // we store them here to reset them in the generateRays method
  if (_prev_t != _t)
  {
    _banked_rays = rayBank();
    _prev_t = _t;

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

  for (auto d : _current_data)
  {
    d.second.points.clear();
    d.second.values.clear();
  }
}


void
TestEMPICStudy::reinitSegment(
    const Elem * elem, const Point & start, const Point & end, const Real length, THREAD_ID tid)
{
  mooseAssert(MooseUtils::absoluteFuzzyEqual((start - end).norm(), length), "Invalid length");
  mooseAssert(currentlyPropagating(), "Should not call while not propagating");

  _fe_problem.setCurrentSubdomainID(elem, tid);

  // If we have any variables or material properties that are active, we definitely need to reinit
  bool reinit = _fe_problem.hasActiveElementalMooseVariables(tid) ||
                _fe_problem.hasActiveMaterialProperties(tid);
  // If not, make sure that the RayKernels have not requested a reinit (this could happen when a
  // RayKernel doesn't have variables or materials but still does an integration and needs qps)
  if (!reinit)
    for (const RayKernelBase * rk : currentRayKernels(tid))
      if (rk->needSegmentReinit())
      {
        reinit = true;
        break;
      }

  if (reinit)
  {
    std::vector<Point> points;
    std::vector<Real> weights;
    buildSegmentQuadrature(start, end, length, points, weights);
    auto & segment_data = _current_data[elem];

    segment_data.points.insert(segment_data.points.begin(), points.begin(), points.end());
    segment_data.values.insert(segment_data.values.begin(), weights.begin(), weights.end());

    std::cout << "Here we go" << std::endl;
    for (unsigned int qp = 0; qp < points.size(); ++qp)
    {
      std::cout << points[qp] << "  " << weights[qp] << std::endl;
    }
      std::cout << std::endl << std::endl;
  }
}

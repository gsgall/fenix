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
#include "KillParticleBC.h"

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
  params.addParam<bool>("replicated_rays", true, "wether or not to have replicated rays");
  return params;
}

TestEMPICStudy::TestEMPICStudy(const InputParameters & parameters)
  : PICStudyBase(parameters),
    _initializer(getUserObject<ParticleInitializerBase>("initializer")),
    _replicated_rays(getParam<bool>("replicated_rays"))
{

  for (const auto elem : *_fe_problem.mesh().getActiveLocalElementRange())
  {
    _current_density_data[elem];
  }
}

void TestEMPICStudy::initialSetup()
{
  PICStudyBase::initialSetup();

  std::vector<RayBoundaryConditionBase *> temporary_bcs;
  getRayBCs(temporary_bcs, _tid);

  for (auto & bc : temporary_bcs)
  {
    auto cast = dynamic_cast<KillParticleBC *>(bc);
    if (cast != nullptr)
      _bcs.push_back(cast);
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
  for (auto & ray : claimed_rays)
  {
    getVelocity(*ray, _temporary_velocity);
    _stepper.setupStep(
        *ray, _temporary_velocity, ray->data()[_charge_index] / ray->data()[_mass_index]);
    setVelocity(*ray, _temporary_velocity);
  }
  moveRaysToBuffer(claimed_rays);
}

void
TestEMPICStudy::postExecuteStudy()
{
  _banked_rays = rayBank();

  for (const auto & bcs : _bcs)
    _dts.merge(bcs->getTransmittableParticleDts());

  comm().set_union(_dts);

  for (const auto & bcs : _bcs)
    _dts.merge(bcs->getLocalParticleDts());

  if (_banked_rays.size() == 0)
    return;

  _banked_rays.erase(
    std::remove_if(
      _banked_rays.begin(),
      _banked_rays.end(),
      [](const std::shared_ptr<Ray> & ray) {
        return ray->distance() != ray->maxDistance();
      }
    )
  );
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
    const auto & ray = (*traceRay(_tid).currentRay());
    auto & segment_data = _current_density_data[elem][ray->id()];
    const auto charge = ray->data(_charge_index);
    const auto weight = ray->data(_weight_index);
    for (const auto & i : index_range(points))
    {
      auto & data = segment_data.emplace_back();
      data.point = points[i];
      data.values = charge * weight * weights[i] * ray->direction();
    }
  }
}

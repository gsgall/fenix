#include "TestPeriodicParticleVelocity.h"

#include "PICStudyBase.h"
#include "ParticleInitializerBase.h"

registerMooseObject("FenixTestApp", TestPeriodicParticleVelocity);

InputParameters
TestPeriodicParticleVelocity::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the Rays");
  return params;
}

TestPeriodicParticleVelocity::TestPeriodicParticleVelocity(const InputParameters & params)
  : GeneralPostprocessor(params), 
    _study(getUserObject<PICStudyBase>("study")), 
    _v_index(_study.getRayDataIndex("v_x"))
{
}

Real
TestPeriodicParticleVelocity::getValue() const
{
  const auto & banked_rays = _study.getBankedRays(); 
  Real mean_velocity = 0.0; 
  unsigned int ray_count = banked_rays.size(); 
  for (const auto & ray : banked_rays)
  {
    mean_velocity += std::abs(ray->data(_v_index)); 
  }

  const auto & periodic_rays = _study.getPeriodicRays();
  ray_count += periodic_rays.size(); 
  for (const auto & ray : periodic_rays)
  {
    mean_velocity += std::abs(ray.velocity(0));
  }
  return mean_velocity / ray_count; 
}

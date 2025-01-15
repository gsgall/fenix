#include "TestPeriodicParticleEnergy.h"

#include "PICStudyBase.h"
#include "ParticleInitializerBase.h"

registerMooseObject("FenixTestApp", TestPeriodicParticleEnergy);

InputParameters
TestPeriodicParticleEnergy::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the Rays");
  params.addParam<std::vector<std::string>>("species", "species that you want to include in the averaging");
  return params;
}

TestPeriodicParticleEnergy::TestPeriodicParticleEnergy(const InputParameters & params)
  : GeneralPostprocessor(params), 
    _study(getUserObject<PICStudyBase>("study")), 
    _v_index(_study.getRayDataIndex("v_x")),
    _mass_index(_study.getRayDataIndex("mass")), 
    _species_index(_study.getRayDataIndex("species")),
    _species(getParam<std::vector<std::string>>("species"))
{
}

void 
TestPeriodicParticleEnergy::initialize()
{
  if (!_species_vals.empty())
    return; 

  for (const auto & s : _species)
    _species_vals.push_back(_study.getSpeciesValue(s)); 
}

Real
TestPeriodicParticleEnergy::getValue() const
{
  const auto & banked_rays = _study.getBankedRays(); 
  Real mean_energy = 0.0; 
  unsigned int ray_count = 0; 
  for (const auto & ray : banked_rays)
  {
    if (std::find(_species_vals.begin(), _species_vals.end(), ray->data(_species_index)) == _species_vals.end())
      continue; 
    mean_energy += 0.5 * ray->data(_mass_index) * ray->data(_v_index) * ray->data(_v_index); 
    ray_count++;
  }

  const auto & periodic_rays = _study.getPeriodicRays();
  for (const auto & ray : periodic_rays)
  {
    if (std::find(_species.begin(), _species.end(), ray.species) == _species.end())
      continue; 
    mean_energy += 0.5 * ray.mass * ray.velocity(0) * ray.velocity(0); 
    ray_count++; 
  }

  std::cout << ray_count << std::endl;
  return mean_energy / ray_count; 
}

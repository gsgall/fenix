#include "AddSurfaceChargingAction.h"

#include "Registry.h"
#include "FEProblem.h"
#include "SurfaceChargeRayBC.h"
#include "SurfaceChargeDensityAccumulator.h"


registerMooseAction("FenixApp", AddSurfaceChargingAction, "add_ray_boundary_condition");
registerMooseAction("FenixApp", AddSurfaceChargingAction, "add_surface_charge");

InputParameters
AddSurfaceChargingAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("This actions creates the RayBC and accumulator needed to properly calculte surface charge deposition");
  params.addRequiredParam<NonlinearVariableName>("variable", "The variable that surface charges will constribute to the residual of");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "boundary", "The list of boundaries (ids or names) from the mesh where surface charge will accumulate");
  params.addParam<UserObjectName>("study", "study", "The PIC study that is tracing the particles");
  params.addParam<std::vector<TagName>>("extra_vector_tags", {}, "The extra tags for the vectors the accumulator should fill");
  return params;
}

AddSurfaceChargingAction::AddSurfaceChargingAction(const InputParameters & params)
  : Action(params),
    _var_name(getParam<NonlinearVariableName>("variable")),
    _study_name(getParam<UserObjectName>("study")),
    _extra_vector_tags(getParam<std::vector<TagName>>("extra_vector_tags")),
    _boundaries(getParam<std::vector<BoundaryName>>("boundary"))
{
}

void
AddSurfaceChargingAction::act()
{
  if (_current_task == "add_surface_charge")
  {
    std::cout << "Adding boundary conditions" << std::endl;
    const auto & type = Registry::getClassName<SurfaceChargeRayBC>();
    auto params = _factory.getValidParams(type);
    // params.set<UserObjectName>("study") = _study_name;
    params.set<std::vector<BoundaryName>>("boundary") = _boundaries;

    _problem->addObject<SurfaceChargeRayBC>(type, "surface_charge_bc", params);

    std::cout << "Adding User objects" << std::endl;
    const auto & type2 = Registry::getClassName<SurfaceChargeDensityAccumulator>();
    params = _factory.getValidParams(type2);
    params.set<NonlinearVariableName>("variable") = _var_name;
    params.set<UserObjectName>("study") = _study_name;

    if (!_extra_vector_tags.empty())
        params.set<std::vector<TagName>>("extra_vector_tags") = _extra_vector_tags;

    _problem->addUserObject(type2, "surface_charge_accumulator", params);
  }
  // else if (_current_task == "add_ray_boundary_condition")
  // {
  // }
}

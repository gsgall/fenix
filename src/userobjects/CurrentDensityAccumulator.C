#include "CurrentDensityAccumulator.h"
#include "ResidualAccumulator.h"
#include "PICStudyBase.h"
#include "MooseMesh.h"

registerMooseObject("FenixApp", CurrentDensityAccumulator);

InputParameters
CurrentDensityAccumulator::validParams()
{
  auto params = GeneralUserObject::validParams();
  params.addClassDescription(
      "Accumulator used to evaluate the inner product of the charge density and the test function "
      "required for solving electromagnetic equations");
  params.addRequiredParam<UserObjectName>("study", "The PICStudy that owns the charged particles");
  // These parameters are necessary when using ResidualAccumulator
  params += TaggingInterface::validParams();
  // This exec flag is necessary for the CurrentDensityAccumulator to contribute to residuals
  ExecFlagEnum & exec_enum = params.set<ExecFlagEnum>("execute_on", true);
  exec_enum.addAvailableFlags(EXEC_PRE_KERNELS);
  params.set<ExecFlagEnum>("execute_on") = EXEC_PRE_KERNELS;
  // making this input parameter private so the user cannot use the object incorrectly
  params.suppressParameter<ExecFlagEnum>("execute_on");
  params.addRequiredParam<std::vector<NonlinearVariableName>>("variables",
                                                 "The variables to contribute to the residual of one for each dimension of the problem");
  return params;
}

CurrentDensityAccumulator::CurrentDensityAccumulator(const InputParameters & params)
  : GeneralUserObject(params),
    _var_names(getParam<std::vector<NonlinearVariableName>>("variables")),
    _study(getUserObject<PICStudyBase>("study")),
    _mesh_dimension(_fe_problem.mesh().dimension())
{
  if (_var_names.size() != _mesh_dimension)
    paramError("variables", "You must have one component of current for each dimension");
}

void
CurrentDensityAccumulator::execute()
{
  if (_fe_problem.currentlyComputingResidual())
  {

    std::vector<std::unique_ptr<FENIX::AccumulatorBase>> accumulators;
    for (uint i = 0; i < _mesh_dimension; ++i)
    {
      accumulators.push_back(std::make_unique<FENIX::ResidualAccumulator>(_fe_problem, this, _var_names[i], 0));
    }

    const auto & current_density_data = _study.getCurrentDensitydata();
    const auto & dt_data = _study.getTimeTakenData();
    for (const auto & elemental_data : current_density_data)
    {
      const auto & elem = elemental_data.first;

      for (const auto & ray_data : elemental_data.second)
      {
        auto it = dt_data.find(ray_data.first);
        Real dt = (it == dt_data.end()) ? _dt : it->second;

        for (const auto & data : ray_data.second)
        {
          for (uint i = 0; i < _mesh_dimension; ++i)
            accumulators[i]->add(*elem, data.point, data.values(i) / dt);
        }
      }
    }

    for (auto & accumulator : accumulators)
      accumulator->finalize();
  }
}

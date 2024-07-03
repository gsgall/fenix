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
  params.addRequiredParam<NonlinearVariableName>("variable",
                                                 "The variable to contribute to the residual of");
  return params;
}

CurrentDensityAccumulator::CurrentDensityAccumulator(const InputParameters & params)
  : GeneralUserObject(params),
    _var_name(getParam<NonlinearVariableName>("variable")),
    _study(getUserObject<PICStudyBase>("study")),
    _charge_index(_study.getRayDataIndex("charge")),
    _weight_index(_study.getRayDataIndex("weight"))
{
}

void
CurrentDensityAccumulator::execute()
{
  if (_fe_problem.currentlyComputingResidual())
  {
    std::unique_ptr<FENIX::AccumulatorBase> accumulator =
        std::make_unique<FENIX::ResidualAccumulator>(_fe_problem, this, _var_name, 0);
    const auto & data = _study.getCurrentDensitydata();
    for (auto & d : data)
    {
      const auto & elem = d.first;
      for (uint i = 0; i < d.second.points.size(); ++i)
      {
        std::cout << d.second.points[i] << "  " << d.second.values[i] << std::endl;
        accumulator->add(*elem, d.second.points[i], d.second.values[i]);

      }
    }

    accumulator->finalize();
  }
}

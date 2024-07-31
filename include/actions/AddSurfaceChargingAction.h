#pragma once

#include "Action.h"

class AddSurfaceChargingAction : public Action
{
public:

  AddSurfaceChargingAction(const InputParameters & params);

  static InputParameters validParams();

  virtual void act();

protected:

  const NonlinearVariableName & _var_name;
  const UserObjectName & _study_name;
  const std::vector<TagName> & _extra_vector_tags;
  const std::vector<BoundaryName> & _boundaries;

};

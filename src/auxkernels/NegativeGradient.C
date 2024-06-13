//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "NegativeGradient.h"

registerMooseObject("FenixApp", NegativeGradient);

InputParameters
NegativeGradient::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription(
      "Returns the negative gradient component of field variable");
  params.addRequiredCoupledVar("var", "The variable of which to take the gradient");
  params.addRequiredRangeCheckedParam<unsigned int>("component",
                              "component < 3",
                               "The component of the component of the gradients to access");
  return params;
}

NegativeGradient::NegativeGradient(const InputParameters & parameters)
  : AuxKernel(parameters),
    _grad_var(coupledGradient("var")),
    _component(getParam<unsigned int>("component"))
{
}

Real
NegativeGradient::computeValue()
{
  return -_grad_var[_qp](_component);
}

//* This file is part of Zapdos, an open-source
//* application for the simulation of plasmas
//* https://github.com/shannon-lab/zapdos
//*
//* Zapdos is powered by the MOOSE Framework
//* https://www.mooseframework.org
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "AuxKernel.h"

/**
 * Constant auxiliary value
 */
class NegativeGradient : public AuxKernel
{
public:
  NegativeGradient(const InputParameters & parameters);

  static InputParameters validParams();

  virtual ~NegativeGradient() {}

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual Real computeValue() override;
  /// The gradient of a coupled variable
  const VariableGradient & _grad_var;
  /// the component of the gradient you will access
  const unsigned int _component;
};

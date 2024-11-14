[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 8
  xmax = 1
[]

[Problem]
  extra_tag_vectors = dump_value
[]

[Functions]
  [rho_func]
    type = ParsedFunction
    expression = '4 * pi^2 * cos(2 * pi * x)'
  []

  [phi_func]
    type = ParsedFunction
    expression = 'cos(2 * pi * x)'
  []
[]

[Variables]
  [phi]
  []
[]

[Kernels]
  [poissons]
    type = ADMatDiffusion
    diffusivity = 1
    variable = phi
  []

  [const_source]
    type = BodyForce
    variable = phi
    function = rho_func
    extra_vector_tags = dump_value
  []
[]

[AuxVariables]
  [rho]
  []

  [phi_solution]
  []

  [Ex]
    order = CONSTANT
    family = MONOMIAL
  []

  [Ex_solution]
    order = CONSTANT
    family = MONOMIAL
  []

  [dump_value]
  []
[]

[AuxKernels]
  [grad_potential_analytic]
    type = NegativeVariableGradientComponent
    variable = Ex_solution
    gradient_variable = phi_solution
    component = 0
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [grad_potential]
    type = NegativeVariableGradientComponent
    variable = Ex
    gradient_variable = phi
    component = 0
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [rho_value]
    type = FunctionAux
    variable = rho
    function = rho_func
  []

  [phi_value]
    type = FunctionAux
    variable = phi_solution
    function = phi_func
  []

  [dumper]
    type = TagVectorAux
    variable = dump_value
    vector_tag = dump_value
    v = phi
  []
[]

[BCs]
  [Periodic]
    [all]
      variable = phi
      auto_direction = 'x'
    []
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]


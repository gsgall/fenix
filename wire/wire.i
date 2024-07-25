[Mesh]
  [file]
    type = FileMeshGenerator
    file = wire.msh
  []
  [element_convert]
    type = ElementOrderConversionGenerator
    input = file
    # conversion_type = SECOND_ORDER
    conversion_type = COMPLETE_ORDER
  []
[]

[Problem]
  solve = false
[]

[Debug]
  show_var_residual_norms = true
[]

[Variables]
  [B]
    # family = RAVIART_THOMAS
    family = NEDELEC_ONE
  []
[]

[Kernels]
  [ampere]
    type = AmperesKernel
    variable = B
  []

  [current_source]
    type = ADCurrentSource
    variable = B
    current_density = J_source_func
  []
[]

# [ICs]
#   [B_ic]
#     type = VectorFunctionIC
#     variable = B
#     function = B_solution_func
#   []
# []
[BCs]
  # [solution]
  #   type = VectorFunctionDirichletBC
  #   variable = B
  #   function = B_solution_func
  # []
  [solution]
    type = AmpereVectorBC
    variable = B
    boundary = 'boundary'
    solution = B_solution_func
  []
[]

[AuxVariables]
  [J_source]
    family = RAVIART_THOMAS
  []

  [B_solution]
    family = RAVIART_THOMAS
  []
[]

[AuxKernels]
  [J_val]
    type = VectorFunctionAux
    variable = J_source
    function = J_source_func
  []

  [B_val]
    type = VectorFunctionAux
    variable = B_solution
    function = B_solution_func
  []
[]

[Functions]
  [r]
    type = ParsedFunction
    expression = 'sqrt(x^2 + y^2)'
  []

  [R]
    type = ConstantFunction
    value = 0.25
  []

  [I]
    type = ParsedFunction
    symbol_names = 'R'
    symbol_values = 'R'
    expression = '2 * pi * R'
  []

  [J]
    type = ParsedFunction
    symbol_names = 'R I'
    symbol_values = 'R I'
    expression = 'I / (pi * R^2)'
  []

  [mu_0]
    type = ConstantFunction
    value = 1
  []

  [J_source_func]
    type = ParsedVectorFunction
    symbol_names = 'r R mu_0 J'
    symbol_values = 'r R mu_0 J'
    expression_z = 'if (r < R, mu_0 * J, 0)'
  []

  [B_inner_mag]
    type = ParsedFunction
    symbol_names = 'mu_0 r I R'
    symbol_values = 'mu_0 r I R'
    expression = 'mu_0 * r *  I / (2 * pi * R^2)'
  []

  [B_outer_mag]
    type = ParsedFunction
    symbol_names = 'mu_0 I r'
    symbol_values = 'mu_0 I r'
    expression = 'mu_0 * I / (2 * pi * r) '
  []

  [B_solution_func]
    type = ParsedVectorFunction
    symbol_names = 'r R B_outer_mag B_inner_mag'
    symbol_values = 'r R B_outer_mag B_inner_mag'
    expression_x = '-y / r * if(r < R, B_inner_mag, B_outer_mag)'
    expression_y = ' x / r * if(r < R, B_inner_mag, B_outer_mag)'
  []

  [imaginary_func]
    type = ParsedVectorFunction
  []
[]


[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]


[Executioner]
  type = Steady
  solve_type = NEWTON
  # nl_rel_tol = 1e-10
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver'
  petsc_options_value = 'lu NONZERO 1.e-9 superlu_dists'
  # line_search = 'none'
  nl_max_its = 15
  l_max_its = 300
  automatic_scaling = true
  compute_scaling_once = false
[]

[Outputs]
  exodus = true

  [nl]
    type = Exodus
    execute_on = 'NONLINEAR'
  []
[]

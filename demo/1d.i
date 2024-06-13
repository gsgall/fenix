# the number of computational particles to be put in each element
particles_per_element = 100
# the analytic solution for the charge density
# set by cli args for each case since it is needed by all
# tests but changes based on the dimension of the problem
charge_density = 1e16

[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 20
    xmax = 0.1
    elem_type = EDGE2
  []
  allow_renumbering = false
[]

[BCs]
  [potential_solution]
    type = DirichletBC
    variable = phi
    value = 0
    boundary = 'left right'
    preset = false
  []
[]

[ICs]
  [potential_ic]
    type = FunctionIC
    variable = phi
    function = '0.5 * (1e16 * 1.6e-19) / 8.85e-12 * ((0.10/2)^2. - (x-0.10/2)^2.)'
  []
[]

[GlobalParams]
  particles_per_element = ${particles_per_element}
[]

[Problem]
  extra_tag_vectors = dump_value
[]

[Variables]
  [phi]
  []

  [n]
  []
[]

[Kernels]
  [poissons]
    type = ADMatDiffusion
    diffusivity = 8.85e-12
    variable = phi
  []

  [projection]
    type = ProjectionKernel
    variable = n
  []
[]

[AuxVariables]
  [Ex]
    order = CONSTANT
    family = MONOMIAL
  []
  [Ey]
  []
  [Ez]
  []
[]

[AuxKernels]
  [grad_potential]
    type = NegativeGradient
    variable = Ex
    var = phi
    component = 0
  []
[]

[Distributions]
  [zero]
    type = Constant
    value = 0.0
  []
[]

[UserObjects]
  [initializer]
    type = ParticlesPerElementInitializer
    mass = 6.64e-26
    charge = 1.602e-19
    number_density = ${charge_density}
    velocity_distributions = 'zero zero zero'
  []

  [velocity_updater]
    type = LeapFrogStepper
    field_components = 'Ex Ey Ez'
  []

  [study]
    type = TestInitializedPICStudy
    initializer = initializer
    velocity_updater = velocity_updater
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on=TIMESTEP_BEGIN
  []

  [potential_accumulator]
    type = ChargeDensityAccumulator
    study = study
    variable = phi
  []

  [density_accumulator]
    type = ChargeDensityAccumulator
    study = study
    variable = n
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[Postprocessors]
  [ray_count]
    type = RayTracingStudyResult
    study = study
    result = 'total_rays_started'
    execute_on = 'TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver'
  petsc_options_value = 'lu NONZERO 1.e-9 superlu_dists'
  line_search = 'none'
  nl_max_its = 15
  l_max_its = 300
  scheme = 'bdf2'
  automatic_scaling = true
  compute_scaling_once = false
  dt = 1e-10
  end_time = 2.5e-8
[]

[Outputs]
  exodus = true
  [csv]
    type = CSV
    execute_on = TIMESTEP_END
  []
  [rays]
    type = RayTracingExodus
    study = study
    output_data_names = 'charge weight mass v_x'
    execute_on = TIMESTEP_END
  []
[]

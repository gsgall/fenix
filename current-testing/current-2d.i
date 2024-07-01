[Problem]
  solve = true
  kernel_coverage_check = false
  extra_tag_vectors = dump_value
[]


[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 1
  ny = 1
  xmin = -1
  xmax = 1
  ymin = -1
  ymax = 1
[]



[Variables]
  [J_x]
  []
  [J_y]
  []
[]

[AuxVariables]
  [dump_value]
  []
[]

[AuxKernels]
  [dump_value]
    type = TagVectorAux
    variable = dump_value
    vector_tag = dump_value
    v = J_x
  []
[]


[Kernels]
  [projection_x]
    type = NullKernel
    variable = J_x
  []
  [projection_y]
    type = NullKernel
    variable = J_y
  []
[]


[Distributions]
  [one]
    type = Constant
    value = 1.5
  []

  [zero]
    type = Constant
    value = 0.0
  []
[]

[UserObjects]
  [initializer]
    type = PlacedParticleInitializer
    mass = 1
    charge = 1
    weight = 1
    start_points = '0 -1 0'
    start_velocities = '0 0 0'
  []

  [velocity_updater]
    type = TestSimpleStepper
  []

  [study]
    type = TestEMPICStudy
    initializer = initializer
    velocity_updater = velocity_updater
    simple_stepper = velocity_updater
    always_cache_traces = true
    data_on_cache_traces = true
    replicated_rays = true
    execute_on = 'TIMESTEP_BEGIN'
  []

  [accumulator]
    type = ChargeDensityAccumulator
    study = study
    variable = J_x
    extra_vector_tags = dump_value
  []
[]

[RayBCs]
  [Kill]
    type = KillRayBC
    boundary = 'left right'
    study = study
  []
[]

[RayKernels]
  [current_x]
    type = NullRayKernel
    # variable = J_x
    # component = 0
  []
[]

[Postprocessors]
  [rays]
    type = RayTracingStudyResult
    study = study
    result = 'total_rays_started'
  []
[]

[Executioner]
  type = Transient
  num_steps = 1
  dt = 1
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver'
  petsc_options_value = 'lu NONZERO 1.e-9 superlu_dists'
  line_search = 'none'
  nl_max_its = 15
  l_max_its = 300
  scheme = 'bdf2'
  automatic_scaling = true
  compute_scaling_once = false
[]

[Outputs]
  exodus = true
  # csv = true
  [nl]
    type = Exodus
    output_nonlinear = true
  []

  [rays]
    type = RayTracingExodus
    study = study
    output_data_names='v_x v_y v_z weight'
    execute_on = 'TIMESTEP_END'
  []
[]

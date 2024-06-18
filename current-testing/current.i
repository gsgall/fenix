[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 10
  ny = 10
  # ny = 4
  # nz = 4
  xmax = 10
  ymax = 10
[]

[Variables]
  [J_x]
  []
  [J_y]
  []
  # [J_z]
  # []
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
    type = ProjectionKernel
    variable = J_x
  []
  [projection_y]
    type = ProjectionKernel
    variable = J_y
  []
  # [projection_z]
  #   type = ProjectionKernel
  #   variable = J_z
  # []
[]

[BCs]
  [zero_x]
    type = DirichletBC
    variable = J_x
    value = 0
    boundary = 'top bottom left right'
  []
  [zero_y]
    type = DirichletBC
    variable = J_y
    value = 0
    boundary = 'top bottom left right'
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
    type = BoundingBoxParticleInitializer
    mass = 1
    charge = 1
    number_density = 2
    particles_per_element = 100
    velocity_distributions = 'one one one'
    bottom_left = '0 0 0'
    top_right = '3 3 0'
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
    replicated_rays = false
    execute_on = "PRE_KERNELS"
  []

[]

[RayBCs]
  [Kill]
    type = KillRayBC
    # boundary = 'left right top bottom front back'
    boundary = 'left right top bottom'
    study = study
  []
[]

[RayKernels]
  [current_x]
    type = CurrentRayKernel
    variable = J_x
    component = 0
    extra_vector_tags = dump_value
  []
  [current_y]
    type = CurrentRayKernel
    variable = J_y
    component = 1
    extra_vector_tags = dump_value
  []
[]

[Executioner]
  type = Transient
  num_steps = 10
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver'
  petsc_options_value = 'lu NONZERO 1.e-9 superlu_dists'
  line_search = 'none'
  nl_max_its = 15
  l_max_its = 300
  scheme = 'bdf2'
  automatic_scaling = true
  compute_scaling_once = false
  dt = 0.5
[]

[Problem]
  solve = true
  kernel_coverage_check = false
  extra_tag_vectors = dump_value
[]

[Outputs]
  exodus = true
  [rays]
    type = RayTracingExodus
    study = study
    output_data_names='v_x v_y v_z weight'
    execute_on = 'TIMESTEP_END'
  []
[]

[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 10
  # ny = 4
  # nz = 4
  xmax = 10
  # ymax = 10
  # zmax = 10
[]

[Variables]
  [J_x]
  []
  # [J_y]
  # []
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
  # [projection_y]
  #   type = ProjectionKernel
  #   variable = J_y
  # []
  # [projection_z]
  #   type = ProjectionKernel
  #   variable = J_z
  # []
[]


[Distributions]
  [one]
    type = Constant
    value = 0.75
  []

  [zero]
    type = Constant
    value = 0.0
  []
[]

[UserObjects]
  [initializer]
    type = ParticlesPerElementInitializer
    mass = 1
    charge = 1
    number_density = 2
    particles_per_element = 100
    velocity_distributions = 'one one one'
    seed = 10
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
    boundary = 'left right'
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
  # [current_y]
  #   type = CurrentRayKernel
  #   variable = J_y
  #   component = 1
  # []
  # [current_z]
  #   type = CurrentRayKernel
  #   variable = J_z
  #   component = 2
  # []
  # [currenty]
  #   type = LineSourceRayKernel
  #   variable = J_y
  #   ray_data_factor_names = 'weight v_y'
  #   extra_vector_tags = dump_value
  # []
[]

[Executioner]
  type = Transient
  num_steps = 10
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

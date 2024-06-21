[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 10
  ny = 10
  # ny = 4
  # nz = 4
  xmax = 10
  ymax = 10
  elem_type = TRI3
[]

[Distributions]
  [one]
    type = Constant
    value = 1
  []

  [zero]
    type = Constant
    value = 0.0
  []
[]

[UserObjects]
  [velocity_updater]
    type = TestSimpleStepper
  []

  [initializer]
    type = BoundingBoxParticleInitializer
    mass = 1
    charge = 1
    number_density = 2
    particles_per_element = 100
    velocity_distributions = 'one one one'
    bottom_left = '0 0 0'
    top_right = '0.1 0 0'
  []
  [study]
    type = TestInjectionPICStudy
    initializer = initializer
    velocity_updater = velocity_updater
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
  []

[]

[RayBCs]
  [Kill]
    type = KillRayBC
    # boundary = 'left right'
    boundary = 'left right top bottom'
    # boundary = 'left right top bottom front back'
    study = study
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[Executioner]
  type = Transient
  num_steps = 22
  dt = 1
[]

[Problem]
  solve = false
  kernel_coverage_check = false
  extra_tag_vectors = dump_value
[]

[Outputs]
  exodus = false
  [rays]
    type = RayTracingExodus
    study = study
    output_data_names='v_x v_y v_z weight'
    execute_on = 'TIMESTEP_END'
  []
[]

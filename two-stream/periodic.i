[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 10
  xmin = 0
  xmax = 1
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [initializer]
    type = TestPlacedParticleInitializer
    start_points = '0.75 0 0'
    start_velocities = '1 0 0'
    mass = 1
    weight = 1
    charge = 1
  []

  [study]
    type = Test1DPeriodicStudy
    stepper = stepper
    initializers = initializer
    use_custom_rayids = false
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs]
  [periodic]
    type = KillRayBC
    boundary = 'left right'
  []
[]

[VectorPostprocessors]
  [ray_data]
    type = TestParticleDataVectorPostprocessor
    additional_ray_data_outputs = 'mass charge weight'
    study = study
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 2
[]

[Problem]
  solve = false
  kernel_coverage_check=false
[]

[Outputs]
  [particle_data]
    type = CSV
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 2
  xmax = 1
[]

[Problem]
  extra_tag_vectors = dump_value
[]

[Variables/n]
[]

[Kernels/null]
  type = NullKernel
  variable = n
[]


[BCs]
  [Periodic]
    [all]
      auto_direction = 'x'
    []
  []
[]

[AuxVariables/dump_value]
[]

[AuxKernels/dump_value]
  type = TagVectorAux
  variable = dump_value
  vector_tag = dump_value
  v = n
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [initializer]
    type = TestPlacedParticleInitializer
    start_points = '0.25 0 0
                    0.25 0 0'
    start_velocities = '0 0 0
                        -0.5 0 0'
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

  [accumulator]
    type = NumberDensityAccumulator
    study = study
    variable = n
    vector_tags = dump_value
  []

  [periodic_accumulator]
    type = PeriodicChargeDensityAccumulator
    study = study
    variable = n
    vector_tags = dump_value
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
    type = TestPeriodicParticleDataVectorPostprocessor
    study = study
    additional_ray_data_outputs = 'charge mass weight'
    execute_on = 'TIMESTEP_END'
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 1
[]

[Outputs]
  exodus = true
  [particle_data]
    type = CSV
    execute_on = 'TIMESTEP_END'
  []
[]

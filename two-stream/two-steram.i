charge_density = 20

[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 64
  xmin = -1
  xmax = 1
[]

[Problem]
  type = FEProblem
  solve = false
[]

# [Variables]
#   [phi]
#   []
# []

# [Kernels]
#   [poissons]
#     type = ADMatDiffusion
#     diffusivity = 1
#     variable = phi
#   []
# []

# [BCs]
#   [Periodic]
#     [all]
#       auto_direction = 'x'
#     []
#   []
# []

[Distributions]
  [zero]
    type = Constant
    value = 0.0
  []

  [left]
    type = Constant
    value = -1
  []

  [right]
    type = Constant
    value = 1
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [left_particles]
    type = PerElementParticleInitializer
    mass = 1
    charge = 1
    number_density = ${charge_density}
    particles_per_element = 10
    velocity_distributions = 'left zero zero'
  []

  [right_particles]
    type = PerElementParticleInitializer
    mass = 1
    charge = 1
    number_density = ${charge_density}
    particles_per_element = 10
    velocity_distributions = 'right zero zero'
  []

  [study]
    type = Test1DPeriodicStudy
    stepper = stepper
    initializers = 'left_particles right_particles'
    # using this for now because of the current
    # hacky implementation of the periodic boundaries
    verify_rays = false
    use_custom_rayids = false
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'TIMESTEP_BEGIN'
  []

  [accumulator]
    type = NumberDensityAccumulator
    study = study
    variable = phi
  []

  [periodic_accumulator]
    type = PeriodicChargeDensityAccumulator
    study = study
    variable = phi
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
  # type = Steady
  # automatic_scaling = true
  # compute_scaling_once = false
  type = Transient
  dt = 1
  num_steps = 10
  automatic_scaling = true
  compute_scaling_once = false

[]

[Outputs]
  [particle_data]
    type = CSV
    execute_on = 'TIMESTEP_END'
  []
[]

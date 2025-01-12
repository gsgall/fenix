[Mesh]
    [gmg]
      type = GeneratedMeshGenerator
      dim = 1
      nx = 32
      xmin = 0
      # maximum domain length is 2 pi
      xmax = 6.28
    []
    allow_renumbering = false
[]

[Problem]
    kernel_coverage_check = false
[]


[Variables]
  [phi]
  []
[]

[BCs]
    [Periodic]
      [all]
        auto_direction = 'x'
      []
    []
[]

[Kernels]
    [poissons]
        type = ADMatDiffusion
        diffusivity = 8.8541878188e-12
        variable = phi
    []

    [background]
        type = BodyForce
        variable = phi
        value = ${fparse -2 * 1.602e-19}
    []
[]


[Distributions]
  [left]
      type = Constant
      value = -2
  []
  [right]
      type = Constant
      value = 2
  []
  [zero]
    type = Constant
    value = 0
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

  [Emag]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [grad_potential]
    type = NegativeVariableGradientComponent
    variable = Ex
    gradient_variable = phi
    component = 0
    execute_on = 'INITIAL TIMESTEP_END'
  []

  [compute_Emag]
    type = ParsedAux
    variable = Emag
    coupled_variables = Ex
    expression = 'abs(Ex)'
  []
[]

[UserObjects]
  [stepper]
    type = LeapFrogStepper
    field_components = 'Ex Ey Ez'
  []

  [left]
      type = PerturbationParticleInitializer
      mass = 6.6464731e-27
      charge = 1.602e-19
      number_density = 1
      particles_per_element = 40
      perturbation_length = 1e-4
      instability_mode = 1
      velocity_distributions = 'left zero zero'
      seed = 1
  []

  [right]
    type = PerturbationParticleInitializer
    mass = 2.1801714e-25
    charge = 1.602e-19
    number_density = 1
    particles_per_element = 40
    perturbation_length = 1e-4
    instability_mode = 1
    velocity_distributions = 'right zero zero'
  []

  [study]
    type = Test1DPeriodicStudy
    stepper = stepper
    initializers = 'left right'
    always_cache_traces = true
    data_on_cache_traces = true
    use_custom_rayids = false
    verify_rays = false
    execute_on = 'TIMESTEP_BEGIN'
  []

  [pot_charge_accumulator]
      type = ChargeDensityAccumulator
      study = study
      variable = phi
  []

  [pot_periodic_charge]
      type = PeriodicChargeDensityAccumulator
      study = study
      variable = phi
  []
[]

[Postprocessors]
  [ray_count]
    type = RayTracingStudyResult
    result = TOTAL_RAYS_STARTED
    study = study
  []

  [Emax]
    type = ElementExtremeValue
    variable = Emag
  []
[]

# [VectorPostprocessors]
#   [rays]
#       type = TestPeriodicParticleDataVectorPostprocessor
#       study = study
#       additional_ray_data_outputs = 'weight charge mass'
#   []
# []

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs/null]
  type = KillRayBC
  study = study
  boundary = 'left right'
[]


[Executioner]
  type = Transient
  end_time = 60
  dt = 0.025
  compute_scaling_once = false
  automatic_scaling = true
[]

[Outputs]
  exodus = true
  csv = true
[]

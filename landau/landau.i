[Mesh]
    [gmg]
      type = GeneratedMeshGenerator
      dim = 1
      nx = 100
      xmin = 0
      xmax = 6.28
    []
    allow_renumbering = false
[]
  
[Problem]
    # solve = false
    kernel_coverage_check = false
[]


[Variables]
  [phi]
  []
  [rho]
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
        diffusivity = 1
        variable = phi
    []

    [background]
        type = BodyForce
        variable = phi 
        value = 1
    []

    [projection]
        type = ProjectionKernel
        variable = rho
    []

    [rho_background]
      type = BodyForce
      variable = rho 
      value = 1
  []
[]


[Distributions]
  [vel]
    type = Normal
    mean = 0
    standard_deviation = 1
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
[]

[AuxKernels]
  [grad_potential]
    type = NegativeVariableGradientComponent
    variable = Ex
    gradient_variable = phi
    component = 0
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[UserObjects]
  [stepper]
    type = LeapFrogStepper
    field_components = 'Ex Ey Ez'
  []

  [initializer]
    type = PerturbationParticleInitializer
    mass = 1
    charge = -1
    number_density = 1
    particles_per_element = 200
    perturbation_length = 1e-2
    instability_mode = 1
    velocity_distributions = 'vel zero zero'
  []

  [study]
    type = Test1DPeriodicStudy
    stepper = stepper
    initializers = 'initializer'
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

  [rho_charge_accumulator]
      type = ChargeDensityAccumulator
      study = study
      variable = rho
  []

  [rho_periodic_charge]
      type = PeriodicChargeDensityAccumulator
      study = study
      variable = rho
  []
  
[]

[Postprocessors]
  [ray_count]
    type = RayTracingStudyResult
    result = TOTAL_RAYS_STARTED
    study = study
  []
[]
[VectorPostprocessors]
  [rays]
      type = TestPeriodicParticleDataVectorPostprocessor 
      study = study
      additional_ray_data_outputs = 'weight charge mass'
  []
[]

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
  # num_steps = 100
  end_time = 150
  dt = 0.1
  compute_scaling_once = false 
  automatic_scaling = true
  # nl_rel_tol = 1e-06
  line_search = none
[]

[Outputs]
  exodus = true 
  csv = true 
[]

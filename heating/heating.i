n = 5.5e7
elemental_charge = 1.602e-19
m_e = 9.1093837e-31
m_Ar = 6.6335209e-26
eps_0 = 8.8541878188e-12
[Mesh]
    [gmg]
      type = GeneratedMeshGenerator
      dim = 1
      nx = 10
      xmin = 0
      # maximum domain length is 2 pi
      # elem_type = EDGE2
      xmax = 100
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
        diffusivity = ${eps_0}
        variable = phi
    []

    # [background]
    #     type = BodyForce
    #     variable = phi
    #     value = ${fparse n * elemental_charge}
    # []
[]


[Distributions]
  [boltz]
      type = Normal
      mean = 0
      standard_deviation = 419382.8812400624
  []
  [zero]
    type = Constant
    value = 0
  []
[]

[UserObjects]
  [stepper]
    type = ElectrostaticLeapFrogStepper
    potential = phi
  []

  [electrons]
      type = PerElementParticleInitializer
      mass = ${m_e}
      charge = ${fparse -elemental_charge}
      number_density = ${n}
      particles_per_element = 40
      velocity_distributions = 'boltz zero zero'
      seed = 1
      species = e
  []

  [ions]
      type = PerElementParticleInitializer
      mass = ${m_Ar}
      charge = ${fparse elemental_charge}
      number_density = ${n}
      particles_per_element = 40
      velocity_distributions = 'zero zero zero'
      species = Ar+
      seed = 10
  []

  [study]
    type = Test1DPeriodicStudy
    stepper = stepper
    initializers = 'electrons ions'
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

  [mean_electron_energy]
    type = TestPeriodicParticleEnergy
    study = study
    species = 'e'
  []

  [mean_ion_energy]
    type = TestPeriodicParticleEnergy
    study = study
    species = 'Ar+'
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
  # end_time = 3e-2
  # dt = 1.1951e-06
  # num_steps = 1
  compute_scaling_once = false
  automatic_scaling = true
  petsc_options = '-snes_converged_reason -snes_linesearch_monitor'
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -ksp_type -snes_linesearch_minlambda'
  petsc_options_value = 'lu NONZERO 1.e-10 fgmres 1e-3'
  nl_rel_tol = 1e-08
[]

[Outputs]
  exodus = false
  csv = true
[]

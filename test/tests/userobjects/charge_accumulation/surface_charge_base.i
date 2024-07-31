[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 5
  xmin = -1
  xmax = 1
[]
[Debug]
  show_actions = true
[]

[Problem]
  extra_tag_vectors = surface_charge_residual
[]

[Variables]
  [rho]
  []
[]

[Kernels]
  [projection]
    type = ProjectionKernel
    variable = rho
  []
[]

[AuxVariables]
  [surface_charge_residual]
  []
[]

[AuxKernels]
  [surface_charge_residual]
    type = TagVectorAux
    variable = surface_charge_residual
    vector_tag = surface_charge_residual
    v = rho
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [study]
    type = TestPICStudyUserDefinedRays
    start_points = '0 0 0
                    -1 0 0'
    start_velocities = '2 0 0
                        1.5 0 0'
    charge = 1.5
    weight = 2
    execute_on = 'TIMESTEP_BEGIN'
    always_cache_traces = true
    data_on_cache_traces = true
    velocity_updater = stepper
  []
[]


[SurfaceCharging]
  [surface_charge]
    variable = rho
    extra_vector_tags = surface_charge_residual
    boundary = 'right'
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 2
[]

[Outputs]
  exodus = true
[]

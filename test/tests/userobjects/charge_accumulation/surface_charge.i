[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 5
  xmin = -1
  xmax = 1
[]

[Problem]
  not_zeroed_tag_vectors = 'surface_charge_residual'
[]

# we have a variable here so we get the system to evaluate all of the residual contributions
# from various different sources but since we change all of the vector tags for the kernels
# there is no non zero value contributed to the residual of this variable
[Variables]
  [unused]
  []
[]

[Kernels]
  [null]
    type = NullKernel
    variable = unused
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
    v = unused
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [study]
    type = TestPICStudyUserDefinedRays
    start_points = '0 0 0
                    -1 0 0
                    0 0 0'
    start_velocities = '2 0 0
                        1.5 0 0
                        0 0 0'
    charge = 1.5
    weight = 2
    execute_on = 'TIMESTEP_BEGIN'
    always_cache_traces = true
    data_on_cache_traces = true
    velocity_updater = stepper
  []

  [accumulator]
    type = SurfaceChargeDensityAccumulator
    study = study
    variable = unused
    vector_tags = surface_charge_residual
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs]
  [surface_charge]
    type = SurfaceChargeRayBC
    boundary = 'right'
  []
[]

[Executioner]
  type = Transient
  dt = 1
  num_steps = 3
[]

[Outputs]
  exodus = true
[]

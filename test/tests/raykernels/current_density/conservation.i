total_time = 1
num_steps = 4
dt = ${fparse total_time / num_steps}
charge = 2
weight = 4
qw = ${fparse charge * weight}

[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 3
  nx = 5
  ny = 5
  nz = 5
  xmax = 2
  ymax = 4
  zmax = 6
[]

[Problem]
  extra_tag_vectors = 'zero_dump rho_0_residual rho_residual J_residual'
[]

# we have a variable here so we get the system to evaluate all of the residual contributions
# from various different sources but since we change all of the vector tags for the kernels
# there is no non zero value contributed to the residual of this variable
[Variables]
  [unused]
  []
[]

# the switch is to help ensure that the initial condition is properly accounted for
# rays only contribute after tracing
[Functions]
  [switch_off]
    type = ParsedFunction
    expression = 'if(t < 1.1 * ${dt}, ${qw}, 0)'
  []
[]
# the dirac kernel exists for the same reason as the function
[DiracKernels]
  [point_source1]
    type = FunctionDiracSource
    variable = unused
    function = switch_off
    point = '0 0 0'
    vector_tags = rho_0_residual
  []
[]

[Kernels]
  [null]
    type = NullKernel
    variable = unused
  []
[]

[AuxVariables]
  [rho_0]
  []
  [rho]
  []
  [J_residual]
  []
  [rho_dot]
  []
  [conservation]
  []
[]

[AuxKernels]
  [rho_0_dump]
    type = TagVectorAux
    variable = rho_0
    vector_tag = rho_0_residual
    v = unused
  []
  [rho_dump]
    type = TagVectorAux
    variable = rho
    vector_tag = rho_residual
    v = unused
  []
  [J_dump]
    type = TagVectorAux
    variable = J_residual
    vector_tag = J_residual
    v = unused
  []
  [rho_dot]
    type = TestTimeDerivativeAuxKernel
    variable = rho_dot
    u = rho
  []
  [conservation]
    type = ParsedAux
    variable = conservation
    coupled_variables = 'rho_0 rho_dot J_residual'
    # rho_0 / ${dt} is here because particles do not
    # contribute to the charge density residual source until after
    # they have been traced
    expression = 'rho_dot - J_residual - rho_0 / ${dt}'
  []
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [study]
    type = TestPICStudyUserDefinedRays
    start_points = '0 0 0'
    start_velocities = '2 4 6'
    charge = ${charge}
    weight = ${weight}
    execute_on = 'TIMESTEP_BEGIN PRE_KERNELS'
    always_cache_traces = true
    data_on_cache_traces = true
    velocity_updater = stepper
    current_density_residual_vector_tag = zero_dump
  []

  [accumulator]
    type = ChargeDensityAccumulator
    study = study
    variable = unused
    vector_tags = rho_residual
  []
[]

[RayKernels]
  [current]
    type = TestChargeConservationRayKernel
    variable = unused
    vector_tags = J_residual
  []
[]

[Executioner]
  type = Transient
  dt = ${dt}
  num_steps = ${num_steps}
[]

[Outputs]
  exodus = true
[]

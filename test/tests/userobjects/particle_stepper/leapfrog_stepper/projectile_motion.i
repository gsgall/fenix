[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 25
  ny = 7
  xmax = 22
  ymax = 7
[]

[Variables]
  [Ex]
  []
  [Ey]
  []
  [Ez]
  []
[]


[Functions]
  [E_x_ic]
    type = ParsedFunction
    expression = '0'
  []
  [E_y_ic]
    type = ParsedFunction
    expression = '-9.81'
  []
  [E_z_ic]
    type = ParsedFunction
    expression = '0'
  []
[]

[ICs]
  [Ex_ic]
    type = FunctionIC
    variable = Ex
    function = E_x_ic
  []
  [Ey_ic]
    type = FunctionIC
    variable = Ey
    function = E_y_ic
  []
  [Ez_ic]
    type = FunctionIC
    variable = Ez
    function = E_z_ic
  []
[]

[UserObjects]
  [stepper]
    type = LeapFrogStepper
    field_components = 'Ex Ey Ez'
  []

  [study]
    type = TestPICStudyUserDefinedRays
    start_points = '0 0 0'
    start_velocities = '10 10 0'
    charge = 1
    mass = 1
    execute_on = 'TIMESTEP_BEGIN'
    always_cache_traces = true
    data_on_cache_traces = true
    stepper = stepper
  []
[]


[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[RayBCs]
  [reflect]
    type = ReflectRayBC
    boundary = 'top bottom right left'
    study = study
  []
[]

[Executioner]
  type = Transient
  dt = 1e-2
  num_steps = 10
[]

[Problem]
  solve = false
  kernel_coverage_check = false
[]

[Outputs/rays]
  type = RayTracingExodus
  study = study
  output_data_names='v_x v_y v_z charge mass'
  execute_on = TIMESTEP_BEGIN
[]

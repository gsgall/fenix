[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 15
  ny = 15
  xmin = -1.5
  xmax = 1.5
  ymin = -1.5
  ymax = 1.5
[]

[Variables]
  [Ex]
  []
  [Ey]
  []
  [Ez]
  []
  [Bx]
  []
  [By]
  []
  [Bz]
  []
[]

[Kernels]
  [Ex_null]
    type = NullKernel
    variable = Ex
  []
  [Ey_null]
    type = NullKernel
    variable = Ey
  []
  [Ez_null]
    type = NullKernel
    variable = Ez
  []
  [Bx_null]
    type = NullKernel
    variable = Bx
  []
  [By_null]
    type = NullKernel
    variable = By
  []
  [Bz_null]
    type = NullKernel
    variable = Bz
  []
[]

[Functions]
  [E_x_ic]
    type = ParsedFunction
    expression = '-1e-2 * x / (x^2 + y^2 + 1e-16)^(-3/2)'
  []
  [E_y_ic]
    type = ParsedFunction
    expression = '-1e-2 * y / (x^2 + y^2 + 1e-16)^(-3/2)'
  []
  [E_z_ic]
    type = ParsedFunction
    expression = '0'
  []
  [B_x_ic]
    type = ParsedFunction
    expression = '0'
  []
  [B_y_ic]
    type = ParsedFunction
    expression = '0'
  []
  [B_z_ic]
    type = ParsedFunction
    expression = 'sqrt(x^2 + y^2)'
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
  [Bx_ic]
    type = FunctionIC
    variable = Bx
    function = B_x_ic
  []
  [By_ic]
    type = FunctionIC
    variable = By
    function = B_y_ic
  []
  [Bz_ic]
    type = FunctionIC
    variable = Bz
    function = B_z_ic
  []
[]

[UserObjects]
  [stepper]
    type = BorisStepper
    efield_components = 'Ex Ey Ez'
    bfield_components = 'Bx By Bz'
  []

  [study]
    type = TestPICStudyUserDefinedRays
    start_points = '0 1 0'
    start_velocities = '1 0 0'
    charge = 1
    mass = 2e-1
    execute_on = 'TIMESTEP_BEGIN'
    always_cache_traces = true
    data_on_cache_traces = true
    velocity_updater = stepper
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
  num_steps = 2700
[]

[VectorPostprocessors]
  [collector]
    type = TestParticleMotionData
    study = study
    execute_on = "TIMESTEP_BEGIN"
  []
[]

[Problem]
  solve = false
[]

[Outputs]
  [data]
    type = CSV
    execute_on = 'FINAL'
  []
  # [rays]
  #   type = RayTracingExodus
  #   study = study
  #   # output_data_names='v_x v_y v_z charge mass'
  #   execute_on = TIMESTEP_BEGIN
  # []
[]

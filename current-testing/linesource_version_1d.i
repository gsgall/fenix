[Problem]
  solve = true
  kernel_coverage_check = false
  extra_tag_vectors = dump_value
[]


[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 1
  nx = 1
  xmin = -1
  xmax = 1
[]



[Variables]
  [J_x]
  []
[]

[AuxVariables]
  [dump_value]
  []
[]

[AuxKernels]
  [dump_value]
    type = TagVectorAux
    variable = dump_value
    vector_tag = dump_value
    v = J_x
  []
[]


[Kernels]
  [projection_x]
    type = ProjectionKernel
    variable = J_x
  []
[]


[UserObjects]
  [study]
    type = RepeatableRayStudy
    names = 'steve'
    start_points = '-1 0 0'
    end_points = '1 0 0'
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = "PRE_KERNELS"
  []
[]

[RayBCs]
  [Kill]
    type = KillRayBC
    boundary = 'left right'
    study = study
  []
[]

[RayKernels]
  [current_x]
    type = LineSourceRayKernel
    variable = J_x
    extra_vector_tags = dump_value
  []
[]

[Postprocessors]
  [rays]
    type = RayTracingStudyResult
    study = study
    result = 'total_rays_started'
  []
[]

[Executioner]
  type = Transient
  num_steps = 1
  dt = 1
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_shift_type -pc_factor_shift_amount -pc_factor_mat_solver'
  petsc_options_value = 'lu NONZERO 1.e-9 superlu_dists'
  line_search = 'none'
  nl_max_its = 15
  l_max_its = 300
  scheme = 'bdf2'
  automatic_scaling = true
  compute_scaling_once = false
[]

[Outputs]
  exodus = true
  csv = true
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = 'TIMESTEP_END'
  []
[]

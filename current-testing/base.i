[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 10
  ny = 10
  xmax = 10
  ymax = 10
  # ny = 5
  # nz = 5
  # xmax = 10
  # ymax = 10
  # zmax = 10
[]

[Variables]
  [J_x]
  []
  [J_y]
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
  [projection]
    type = ProjectionKernel
    variable = J_x
  []
  [projectiony]
    type = ProjectionKernel
    variable = J_y
  []
[]

[UserObjects]
  [study]
    type = RepeatableRayStudy
    names = 'test'
    start_points = '1 1 0'
    end_points = '2 2 0'
    ray_data_names = 'weight v_x v_y'
    initial_ray_data = '1 1 1'
    always_cache_traces = true
    data_on_cache_traces = true
    execute_on = 'PRE_KERNELS'
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
  [current]
    type = LineSourceRayKernel
    variable = J_x
    ray_data_factor_names = 'weight v_x'
    extra_vector_tags = dump_value
  []
  [currenty]
    type = LineSourceRayKernel
    variable = J_y
    ray_data_factor_names = 'weight v_y'
    extra_vector_tags = dump_value
  []
[]

[Executioner]
  type = Transient
  num_steps = 1
  dt = 1
[]

[Problem]
  solve = true
  kernel_coverage_check = false
  extra_tag_vectors = dump_value
[]

[Outputs]
  exodus = true
  [rays]
    type = RayTracingExodus
    study = study
    output_data_names='v_x v_y weight'
    execute_on = 'TIMESTEP_END'
  []
[]

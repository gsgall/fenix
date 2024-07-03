
[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 1
  ny = 1
  xmin = -1
  xmax = 1
  ymin = -1
  ymax = 1
[]

[Problem]
  extra_tag_vectors = dump_value
[]

[Variables]
  [phi]
  []
[]

[Kernels]
  [poissons]
    type = ProjectionKernel
    variable = phi
  []
[]

[BCs/zero]
  type = DirichletBC
  variable = phi
  value = 0
  boundary = 'left right'
  preset = false
[]

[AuxVariables/dump_value]
[]

[AuxKernels/dump_value]
  type = TagVectorAux
  variable = dump_value
  vector_tag = dump_value
  v = phi
[]

[UserObjects]
  [stepper]
    type = TestSimpleStepper
  []

  [study]
    type = TestPICStudyUserDefinedRays
    start_points = '-0.57735 -1 0
                     0.57735 -1 0'

    start_velocities = '0 0 0
                        0 0 0'

    charge = 1
    weight = 1
    execute_on = 'TIMESTEP_BEGIN'
    always_cache_traces = true
    data_on_cache_traces = true
    velocity_updater = stepper
  []

  [accumulator]
    type = ChargeDensityAccumulator
    study = study
    variable = phi
    extra_vector_tags = dump_value
  []
[]

[RayBCs]
  [reflect]
    type = ReflectRayBC
    boundary = 'left right'
    study = study
  []
[]

[RayKernels]
  [null]
    type = NullRayKernel
  []
[]

[Executioner]
  type = Transient
  num_steps = 1
[]

[Outputs]
  exodus = true
[]

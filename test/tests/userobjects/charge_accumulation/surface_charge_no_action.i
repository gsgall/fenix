!include surface_charge_base.i

[UserObjects]
  [accumulator]
    type = SurfaceChargeDensityAccumulator
    variable = rho
    extra_vector_tags = surface_charge_residual
  []
[]

[RayBCs]
  [surface_charge]
    type = SurfaceChargeRayBC
    boundary = 'right'
  []
[]


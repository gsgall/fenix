!include surface_charge_base.i

[SurfaceCharging]
  [surface_charge]
    variable = rho
    extra_vector_tags = surface_charge_residual
    boundary = 'right'
  []
[]

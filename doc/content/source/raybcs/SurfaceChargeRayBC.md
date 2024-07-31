# SurfaceChargeRayBC.md

!syntax description /RayBCs/SurfaceChargeRayBC

## Overview

On its own this boundary condition is just a [KillRayBC](KillRayBC) that maintains a list of all of the particles which have hit a given boundary during the simulation and the location where they hit the boundary. In order for killed particles to actually contribute to the residual of a variable as a charge density source term the [SurfaceChargeDensityAccumulator](SurfaceChargeDensityAccumulator) must also exist within a simulation. Please see [SurfaceChargeDensityAccumulator](SurfaceChargeDensityAccumulator) for more details.

## Example Input Syntax

!listing test/tests/userobjects/charge_accumulation/surface_charge.i

!syntax parameters /RayBCs/SurfaceChargeRayBC

!syntax inputs /RayBCs/SurfaceChargeRayBC

!syntax children /RayBCs/SurfaceChargeRayBC

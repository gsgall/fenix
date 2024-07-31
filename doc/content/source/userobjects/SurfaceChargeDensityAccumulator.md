# SurfaceChargeDensityAccumulator.md

!syntax description /UserObjects/SurfaceChargeDensityAccumulator

## Overview

This accumulator performs that same task as the [ChargeDensityAccumulator.md], except it only contributes to a variable's residual from particles that have been killed by a [SurfaceChargeRayBC.md].

## Example Input Syntax

!listing test/tests/userobjects/charge_accumulation/surface_charge.i

!syntax parameters /UserObjects/SurfaceChargeDensityAccumulator

!syntax inputs /UserObjects/SurfaceChargeDensityAccumulator

!syntax children /UserObjects/SurfaceChargeDensityAccumulator

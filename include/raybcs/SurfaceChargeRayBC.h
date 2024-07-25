#pragma once

#include "KillRayBC.h"
#include "SurfaceChargeRayBC.h"

struct SurfaceChargeData
{
  Real value;
  Point point;
};

class SurfaceChargeRayBC : public KillRayBC
{
public:
  SurfaceChargeRayBC(const InputParameters & params);

  static InputParameters validParams();

  virtual void onBoundary(const unsigned int num_applying) override;

  std::unordered_map<const Elem*, std::vector<SurfaceChargeData>> surfaceChargeData()
  {
    return std::move(_charge_data);
  }
protected:

  const RayDataIndex _charge_index;
  /// the ray data index where the weight of the particle is stored
  const RayDataIndex _weight_index;

  std::unordered_map<const Elem *, std::vector<SurfaceChargeData>> _charge_data;


};

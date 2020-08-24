#pragma once
#include <bundle/bundle_adjuster.h>
#include <map/ground_control_points.h>
#include <map/map.h>
#include <pybind11/pybind11.h>

#include <unordered_set>

namespace py = pybind11;
namespace map {
class GroundControlPoint;
}
class OpenSfMConfig;
class BAHelpers {
 public:
  static py::dict Bundle(
      map::Map& map,
      const std::unordered_map<map::CameraId, Camera>& camera_priors,
      const AlignedVector<map::GroundControlPoint>& gcp,
      const py::dict& config);

  static py::tuple BundleLocal(
      map::Map& map,
      const std::unordered_map<map::CameraId, Camera>& camera_priors,
      const AlignedVector<map::GroundControlPoint>& gcp,
      const map::ShotId& central_shot_id,
      const py::dict& config);

  static std::pair<std::unordered_set<map::Shot*>,
                   std::unordered_set<map::Shot*>>
  ShotNeighborhood(map::Map& map, const map::ShotId& central_shot_id,
                   const size_t radius, const size_t min_common_points,
                   const size_t max_interior_size);
  static std::string DetectAlignmentConstraints(
      const map::Map& map, const py::dict& config,
      const AlignedVector<map::GroundControlPoint>& gcp);
 private:
  static std::unordered_set<map::Shot*> DirectShotNeighbors(
      map::Map& map, const std::unordered_set<map::Shot*>& shot_ids,
      const size_t min_common_points, const size_t max_neighbors);
  static void AddGCPToBundle(
      BundleAdjuster& ba, const AlignedVector<map::GroundControlPoint>& gcp,
      const std::unordered_map<map::ShotId, map::Shot>& shots);
  static bool TriangulateGCP(
      const map::GroundControlPoint& point,
      const std::unordered_map<map::ShotId, map::Shot>& shots,
      Vec3d& coordinates);

  static void AlignmentConstraints(
    const map::Map& map, const py::dict& config,
    const AlignedVector<map::GroundControlPoint>& gcp,
    MatX3d& Xp, MatX3d& X);


};

#include "reconstruction/manager.h"
#include "reconstruction/point.h"
#include "reconstruction/camera.h"
#include "reconstruction/shot.h"
#include "reconstruction/pose.h"

namespace reconstruction
{

void 
ReconstructionManager::AddObservation(Shot *const shot,  Point *const point, const FeatureId feat_id) const
{
  shot->AddPointObservation(point, feat_id);
  point->AddObservation(shot, feat_id);
}

void
ReconstructionManager::RemoveObservation(Shot *const shot,  Point *const point, const FeatureId feat_id) const
{
  shot->RemovePointObservation(feat_id);
  point->RemoveObservation(shot);
}

Shot*
ReconstructionManager::CreateShot(const ShotId shot_id, const CameraId camera_id, const Pose& pose, const std::string& name)
{
  const auto& shot_cam = cameras_.at(camera_id);
  auto it = shots_.emplace(shot_id, std::make_unique<Shot>(shot_id, *shot_cam, pose, name));
  
  // Insert failed
  if (!it.second)
  {
    return nullptr;

  }

  if (!name.empty())
  {  
    shot_names_.emplace(name, shot_id);
  }
  return it.first->second.get();
}

void
ReconstructionManager::UpdateShotPose(const ShotId shot_id, const Pose& pose)
{
  shots_.at(shot_id)->SetPose(pose);
}

void 
ReconstructionManager::RemoveShot(const ShotId shot_id)
{
    //1) Find the point
  const auto& shot_it = shots_.find(shot_id);
  if (shot_it != shots_.end())
  {
    const auto& shot = shot_it->second;
    //2) Remove it from all the points
    for (const auto& point : shot->GetPoints())
    {
      if (point != nullptr)
      {
        point->RemoveObservation(shot.get());
      }
    }

    //3) Remove from shot_names
    shot_names_.erase(shot->shot_name_);

    //4) Remove from shots
    shots_.erase(shot_it);
  }
}

Point*
ReconstructionManager::CreatePoint(const PointId point_id, const Eigen::Vector3d& global_pos, const std::string& name)
{

  auto it = points_.emplace(point_id, std::make_unique<Point>(point_id, global_pos, name));
  
  // Insert failed
  if (!it.second)
  {
    return nullptr;
  }

  if (!name.empty())
  {  
    point_names_.emplace(name, point_id);
  }
  
  return it.first->second.get(); //the raw pointer
}

void
ReconstructionManager::UpdatePoint(const PointId point_id, const Eigen::Vector3d& global_pos)
{
  points_.at(point_id)->SetGlobalPos(global_pos);
}

void 
ReconstructionManager::RemovePoint(const PointId point_id)
{
  //1) Find the point
  const auto& point_it = points_.find(point_id);
  if (point_it != points_.end())
  {
    const auto& point = point_it->second;
    //2) Remove all its observation
    const auto& observations = point->GetObservations();
    for (const auto& obs : observations)
    {
      Shot* shot = obs.first;
      const auto feat_id = obs.second;
      shot->RemovePointObservation(feat_id);
    }

    //3) Remove from point_names
    point_names_.erase(point->point_name_);

    //4) Remove from points
    points_.erase(point_it);
  }
}
};

// Copyright 2021 Apex.AI, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef OBJECT_DETECTION__DETECTED_OBJECTS_DISPLAY_HPP_
#define OBJECT_DETECTION__DETECTED_OBJECTS_DISPLAY_HPP_

#include <object_detection/object_polygon_display_base.hpp>

#include <autoware_auto_perception_msgs/msg/detected_object.hpp>
#include <autoware_auto_perception_msgs/msg/detected_objects.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <memory>

namespace autoware
{
namespace rviz_plugins
{
namespace object_detection
{
/// \brief Class defining rviz plugin to visualize DetectedObjects
class AUTOWARE_AUTO_PERCEPTION_RVIZ_PLUGIN_PUBLIC DetectedObjectsDisplay
: public ObjectPolygonDisplayBase<autoware_auto_perception_msgs::msg::DetectedObjects>
{
  Q_OBJECT

public:
  using DetectedObjects = autoware_auto_perception_msgs::msg::DetectedObjects;

  DetectedObjectsDisplay();

private:
  void processMessage(DetectedObjects::ConstSharedPtr msg) override;
  void processPointCloud(
    const DetectedObjects::ConstSharedPtr & input_objs_msg,
    const sensor_msgs::msg::PointCloud2::ConstSharedPtr & input_pointcloud_msg);
};

}  // namespace object_detection
}  // namespace rviz_plugins
}  // namespace autoware

#endif  // OBJECT_DETECTION__DETECTED_OBJECTS_DISPLAY_HPP_

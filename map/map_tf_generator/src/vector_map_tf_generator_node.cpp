// Copyright 2022 TIER IV, Inc.
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

#include <lanelet2_extension/utility/message_conversion.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_map_msgs/msg/lanelet_map_bin.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/Point.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/static_transform_broadcaster.h>

#include <memory>
#include <string>

class VectorMapTFGeneratorNode : public rclcpp::Node
{
public:
  explicit VectorMapTFGeneratorNode(const rclcpp::NodeOptions & options)
  : Node("vector_map_tf_generator", options)
  {
    map_frame_ = declare_parameter("map_frame", "map");
    viewer_frame_ = declare_parameter("viewer_frame", "viewer");

    sub_ = create_subscription<autoware_map_msgs::msg::LaneletMapBin>(
      "vector_map", rclcpp::QoS{1}.transient_local(),
      std::bind(&VectorMapTFGeneratorNode::onVectorMap, this, std::placeholders::_1));

    static_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);
  }

private:
  std::string map_frame_;
  std::string viewer_frame_;
  rclcpp::Subscription<autoware_map_msgs::msg::LaneletMapBin>::SharedPtr sub_;

  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_broadcaster_;
  std::shared_ptr<lanelet::LaneletMap> lanelet_map_ptr_;

  void onVectorMap(const autoware_map_msgs::msg::LaneletMapBin::ConstSharedPtr msg)
  {
    lanelet_map_ptr_ = std::make_shared<lanelet::LaneletMap>();
    lanelet::utils::conversion::fromBinMsg(*msg, lanelet_map_ptr_);
    std::vector<double> points_x;
    std::vector<double> points_y;
    std::vector<double> points_z;

    for (const lanelet::Point3d & point : lanelet_map_ptr_->pointLayer) {
      const double point_x = point.x();
      const double point_y = point.y();
      const double point_z = point.z();
      points_x.push_back(point_x);
      points_y.push_back(point_y);
      points_z.push_back(point_z);
    }
    const double coordinate_x =
      std::accumulate(points_x.begin(), points_x.end(), 0.0) / points_x.size();
    const double coordinate_y =
      std::accumulate(points_y.begin(), points_y.end(), 0.0) / points_y.size();
    const double coordinate_z =
      std::accumulate(points_z.begin(), points_z.end(), 0.0) / points_z.size();

    geometry_msgs::msg::TransformStamped static_transformStamped;
    static_transformStamped.header.stamp = this->now();
    static_transformStamped.header.frame_id = map_frame_;
    static_transformStamped.child_frame_id = viewer_frame_;
    static_transformStamped.transform.translation.x = coordinate_x;
    static_transformStamped.transform.translation.y = coordinate_y;
    static_transformStamped.transform.translation.z = coordinate_z;
    tf2::Quaternion quat;
    quat.setRPY(0, 0, 0);
    static_transformStamped.transform.rotation.x = quat.x();
    static_transformStamped.transform.rotation.y = quat.y();
    static_transformStamped.transform.rotation.z = quat.z();
    static_transformStamped.transform.rotation.w = quat.w();

    static_broadcaster_->sendTransform(static_transformStamped);

    RCLCPP_INFO_STREAM(
      get_logger(), "broadcast static tf. map_frame:"
                      << map_frame_ << ", viewer_frame:" << viewer_frame_ << ", x:" << coordinate_x
                      << ", y:" << coordinate_y << ", z:" << coordinate_z);
  }
};

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(VectorMapTFGeneratorNode)

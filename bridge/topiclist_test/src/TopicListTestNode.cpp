#include <rclcpp/node.hpp>
#include <gz/transport/Node.hh>

namespace topiclist_test_node
{

class TopicListTestNode : public rclcpp::Node
{
public:
  explicit TopicListTestNode(const rclcpp::NodeOptions & options);
  ~TopicListTestNode() = default;

private:
  void timer_callback();
  std::shared_ptr<gz::transport::Node> gz_node_;
  rclcpp::TimerBase::SharedPtr timer_;
  double frequency_;
  std::vector<std::string> expected_topics_ = {
    "/clock",
    "/gazebo/resource_paths",
    "/imu",
    "/model/vehicle/odometry",
    "/model/vehicle/tf",
    "/rgbd_camera/camera_info",
    "/rgbd_camera/depth_image",
    "/rgbd_camera/image",
    "/rgbd_camera/points",
    "/sensors/marker",
    "/stats",
    "/world/default/clock",
    "/world/default/dynamic_pose/info",
    "/world/default/pose/info",   
    "/world/default/scene/deletion",
    "/world/default/scene/info",
    "/world/default/state",
    "/world/default/stats",
    "/model/vehicle/cmd_vel",
    "/model/vehicle/enable",
    "/world/default/light_config",
    "/world/default/material_color"
  };

  std::vector<std::string> expected_services_ = {
    "/gazebo/resource_paths/add",
    "/gazebo/resource_paths/get",
    "/gazebo/resource_paths/resolve",
    "/gazebo/worlds",
    "/imu/set_rate",
    "/rgbd_camera/set_rate",
    "/sensors/marker",
    "/sensors/marker/list",
    "/sensors/marker_array",
    "/server_control",
    "/world/default/control",
    "/world/default/control/state",
    "/world/default/create",
    "/world/default/create/blocking",
    "/world/default/create_multiple",
    "/world/default/create_multiple/blocking",
    "/world/default/declare_parameter",
    "/world/default/disable_collision",
    "/world/default/disable_collision/blocking",
    "/world/default/enable_collision",
    "/world/default/enable_collision/blocking",
    "/world/default/entity/system/add",
    "/world/default/generate_world_sdf",
    "/world/default/get_parameter",
    "/world/default/gui/info",
    "/world/default/level/set_performer",
    "/world/default/light_config",
    "/world/default/light_config/blocking",
    "/world/default/list_parameters",
    "/world/default/playback/control",
    "/world/default/remove",
    "/world/default/remove/blocking",
    "/world/default/scene/graph",
    "/world/default/scene/info",
    "/world/default/set_parameter",
    "/world/default/set_physics",
    "/world/default/set_physics/blocking",
    "/world/default/set_pose",
    "/world/default/set_pose/blocking",
    "/world/default/set_pose_vector",
    "/world/default/set_pose_vector/blocking",
    "/world/default/set_spherical_coordinates",
    "/world/default/set_spherical_coordinates/blocking",
    "/world/default/state",
    "/world/default/state_async",
    "/world/default/system/info",
    "/world/default/visual_config",
    "/world/default/visual_config/blocking",
    "/world/default/wheel_slip",
    "/world/default/wheel_slip/blocking",
  };
};

TopicListTestNode::TopicListTestNode(const rclcpp::NodeOptions & options)
: rclcpp::Node("topiclist_test_node", options)
{
  gz_node_ = std::make_shared<gz::transport::Node>();
  
  this->declare_parameter<double>("frequency", 1.0);
  this->get_parameter("frequency", frequency_);

  RCLCPP_INFO(this->get_logger(), "\033[1;30;43mTopicListTestNode frequency: %f Hz\033[0m", frequency_);

  timer_ = this->create_wall_timer(
    std::chrono::milliseconds(static_cast<int>(1000.0 / frequency_)),
    std::bind(&TopicListTestNode::timer_callback, this));
}

void TopicListTestNode::timer_callback()
{
  std::vector<std::string> topics;
  gz_node_->TopicList(topics);

  RCLCPP_INFO(this->get_logger(),
    "\033[1;43mExpected Gazebo topics: %zu, discovered by TopicList(): %zu\033[0m",
    expected_topics_.size(), topics.size());
  
  std::stringstream ss;
  ss << "Missing topics:";
  for (const auto & expected_topic : expected_topics_) 
  {
    if (std::find(topics.begin(), topics.end(), expected_topic) == topics.end()) 
    {
      ss << std::endl << "  " << expected_topic;
    }
  }
  ss << std::endl << "Extra topics:";
  for (const auto & topic : topics)
  {
    if (std::find(expected_topics_.begin(), expected_topics_.end(), topic) == expected_topics_.end())
    {
      ss << std::endl << "  " << topic;
    }
  }
  RCLCPP_INFO(this->get_logger(), "%s", ss.str().c_str());

  std::vector<std::string> services;
  gz_node_->ServiceList(services);
  RCLCPP_INFO(this->get_logger(),
    "\033[1;44mExpected Gazebo services: %zu, discovered by ServiceList(): %zu\033[0m",
    expected_services_.size(), services.size());
  ss.str("");
  ss << "Missing services:";
  for (const auto & expected_service : expected_services_)
  {
    if (std::find(services.begin(), services.end(), expected_service) == services.end())
    {
      ss << std::endl << "  " << expected_service;
    }
  }
  ss << std::endl << "Extra services:";
  for (const auto & service : services)
  {
    if (std::find(expected_services_.begin(), expected_services_.end(), service) == expected_services_.end())
    {
      ss << std::endl << "  " << service;
    }
  }
  RCLCPP_INFO(this->get_logger(), "%s", ss.str().c_str());
}

} // namespace topiclist_test_node

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(topiclist_test_node::TopicListTestNode)

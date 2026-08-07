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
    "/gui/camera/pose",
    "/gui/currently_tracked",
    "/gui/track",
    "/imu",
    "/model/vehicle/odometry",
    "/model/vehicle/tf",
    "/rgbd_camera/camera_info",
    "/rgbd_camera/depth_image",
    "/rgbd_camera/image",
    "/rgbd_camera/points",
    "/sensors/marker",
    "/stats",
    "/world/test_world/clock",
    "/world/test_world/dynamic_pose/info",
    "/world/test_world/pose/info",   
    "/world/test_world/scene/deletion",
    "/world/test_world/scene/info",
    "/world/test_world/state",
    "/world/test_world/stats",
    "/model/vehicle/cmd_vel",
    "/model/vehicle/enable",
    "/world/test_world/light_config",
    "/world/test_world/material_color"
  };

  std::vector<std::string> expected_services_ = {
    "/gazebo/resource_paths/add",
    "/gazebo/resource_paths/get",
    "/gazebo/resource_paths/resolve",
    "/gazebo/worlds",
    "/gui/camera/view_control",
    "/gui/camera/view_control/reference_visual",
    "/gui/camera/view_control/sensitivity",
    "/gui/copy",
    "/gui/move_to",
    "/gui/move_to/pose",
    "/gui/paste",
    "/gui/screenshot",
    "/gui/view/collisions",
    "/gui/view/com",
    "/gui/view/frames",
    "/gui/view/inertia",
    "/gui/view/joints",
    "/gui/view/transparent",
    "/gui/view/wireframes",
    "/imu/set_rate",
    "/marker",
    "/marker/list",
    "/marker_array",
    "/rgbd_camera/set_rate",
    "/sensors/marker",
    "/sensors/marker/list",
    "/sensors/marker_array",
    "/server_control",
    "/world/test_world/control",
    "/world/test_world/control/state",
    "/world/test_world/create",
    "/world/test_world/create/blocking",
    "/world/test_world/create_multiple",
    "/world/test_world/create_multiple/blocking",
    "/world/test_world/declare_parameter",
    "/world/test_world/disable_collision",
    "/world/test_world/disable_collision/blocking",
    "/world/test_world/enable_collision",
    "/world/test_world/enable_collision/blocking",
    "/world/test_world/entity/system/add",
    "/world/test_world/generate_world_sdf",
    "/world/test_world/get_parameter",
    "/world/test_world/gui/info",
    "/world/test_world/level/set_performer",
    "/world/test_world/light_config",
    "/world/test_world/light_config/blocking",
    "/world/test_world/list_parameters",
    "/world/test_world/playback/control",
    "/world/test_world/remove",
    "/world/test_world/remove/blocking",
    "/world/test_world/scene/graph",
    "/world/test_world/scene/info",
    "/world/test_world/set_parameter",
    "/world/test_world/set_physics",
    "/world/test_world/set_physics/blocking",
    "/world/test_world/set_pose",
    "/world/test_world/set_pose/blocking",
    "/world/test_world/set_pose_vector",
    "/world/test_world/set_pose_vector/blocking",
    "/world/test_world/set_spherical_coordinates",
    "/world/test_world/set_spherical_coordinates/blocking",
    "/world/test_world/state",
    "/world/test_world/state_async",
    "/world/test_world/system/info",
    "/world/test_world/visual_config",
    "/world/test_world/visual_config/blocking",
    "/world/test_world/wheel_slip",
    "/world/test_world/wheel_slip/blocking",
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

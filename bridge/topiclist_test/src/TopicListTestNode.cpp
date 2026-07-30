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
    "/world/multi_robot/clock",
    "/world/multi_robot/dynamic_pose/info",
    "/world/multi_robot/pose/info",   
    "/world/multi_robot/scene/deletion",
    "/world/multi_robot/scene/info",
    "/world/multi_robot/state",
    "/world/multi_robot/stats",
    "/model/vehicle/cmd_vel",
    "/model/vehicle/enable",
    "/world/multi_robot/light_config",
    "/world/multi_robot/material_color"
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
  RCLCPP_INFO(this->get_logger(), "%s", ss.str().c_str());
}

} // namespace topiclist_test_node

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(topiclist_test_node::TopicListTestNode)

#include <rclcpp/rclcpp.hpp>
#include <rclcpp/node.hpp>
#include <gz/transport/Node.hh>

#include <rosgraph_msgs/msg/clock.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2_msgs/msg/tf_message.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/bool.hpp>


namespace ros_pub_sub_node
{

class RosPubSubNode : public rclcpp::Node
{
public:
  explicit RosPubSubNode(const rclcpp::NodeOptions & options);
  ~RosPubSubNode() = default;

private:
  template<typename T>
  typename rclcpp::Subscription<T>::SharedPtr create_sub(
    const std::string & topic_name,
    const rclcpp::QoS & qos = rclcpp::QoS(10));
  std::vector<rclcpp::SubscriptionBase::SharedPtr> subs_;
  std::vector<rclcpp::PublisherBase::SharedPtr> pubs_;
  
};

template<typename T>
typename rclcpp::Subscription<T>::SharedPtr RosPubSubNode::create_sub(
  const std::string & topic_name,
  const rclcpp::QoS & qos)
{
  RCLCPP_INFO(this->get_logger(), "Creating subscription for topic: %s", topic_name.c_str());

  auto sub = this->create_subscription<T>(
    topic_name,
    qos,
    [this, topic_name](const typename T::SharedPtr msg) {
      RCLCPP_INFO_ONCE(this->get_logger(), "Received message on topic: %s", topic_name.c_str());
    });

  return sub;
}

RosPubSubNode::RosPubSubNode(const rclcpp::NodeOptions & options)
: rclcpp::Node("ros_pub_sub_node", options)
{
  bool clock_sub_enable = this->declare_parameter("clock_sub.enable", false);
  if (clock_sub_enable)
  {
    std::string clock_sub_topic = this->declare_parameter("clock_sub.topic", "/clock");
    subs_.push_back(create_sub<rosgraph_msgs::msg::Clock>(clock_sub_topic));
  }

  bool imu_sub_enable = this->declare_parameter("imu_sub.enable", false);
  if (imu_sub_enable)
  {
    std::string imu_sub_topic = this->declare_parameter("imu_sub.topic", "/imu");
    subs_.push_back(create_sub<sensor_msgs::msg::Imu>(imu_sub_topic));
  }

  bool camera_info_sub_enable = this->declare_parameter("rgbd_camera.camera_info_sub.enable", false);
  if (camera_info_sub_enable)
  {
    std::string camera_info_sub_topic = this->declare_parameter("rgbd_camera.camera_info_sub.topic", "/rgbd_camera/camera_info");
    subs_.push_back(create_sub<sensor_msgs::msg::CameraInfo>(camera_info_sub_topic));
  }

  bool depth_image_sub_enable = this->declare_parameter("rgbd_camera.depth_image_sub.enable", false);
  if (depth_image_sub_enable)
  {
    std::string depth_image_sub_topic = this->declare_parameter("rgbd_camera.depth_image_sub.topic", "/rgbd_camera/depth_image");
    subs_.push_back(create_sub<sensor_msgs::msg::Image>(depth_image_sub_topic));
  }

  bool image_sub_enable = this->declare_parameter("rgbd_camera.image_sub.enable", false);
  if (image_sub_enable)
  {
    std::string image_sub_topic = this->declare_parameter("rgbd_camera.image_sub.topic", "/rgbd_camera/image");
    subs_.push_back(create_sub<sensor_msgs::msg::Image>(image_sub_topic));
  }

  bool points_sub_enable = this->declare_parameter("rgbd_camera.points_sub.enable", false);
  if (points_sub_enable)
  {
    std::string points_sub_topic = this->declare_parameter("rgbd_camera.points_sub.topic", "/rgbd_camera/points");
    subs_.push_back(create_sub<sensor_msgs::msg::PointCloud2>(points_sub_topic));
  }

  bool odom_sub_enable = this->declare_parameter("vehicle.odom_sub.enable", false);
  if (odom_sub_enable)
  {
    std::string odom_sub_topic = this->declare_parameter("vehicle.odom_sub.topic", "/model/vehicle/odometry");
    subs_.push_back(create_sub<nav_msgs::msg::Odometry>(odom_sub_topic));
  }

  bool tf_sub_enable = this->declare_parameter("vehicle.tf_sub.enable", false);
  if (tf_sub_enable)
  {
    std::string tf_sub_topic = this->declare_parameter("vehicle.tf_sub.topic", "/model/vehicle/tf");
    subs_.push_back(create_sub<tf2_msgs::msg::TFMessage>(tf_sub_topic));
  }

  bool cmd_vel_pub_enable = this->declare_parameter("vehicle.cmd_vel_pub.enable", false);
  if (cmd_vel_pub_enable)
  {
    std::string cmd_vel_pub_topic = this->declare_parameter("vehicle.cmd_vel_pub.topic", "/model/vehicle/cmd_vel");
    pubs_.push_back(this->create_publisher<geometry_msgs::msg::Twist>(cmd_vel_pub_topic, 10));
    RCLCPP_INFO(this->get_logger(), "Created publisher for topic: %s", cmd_vel_pub_topic.c_str());
  }
  bool enable_pub_enable = this->declare_parameter("vehicle.enable_pub.enable", false);
  if (enable_pub_enable)
  {
    std::string enable_pub_topic = this->declare_parameter("vehicle.enable_pub.topic", "/model/vehicle/enable");
    pubs_.push_back(this->create_publisher<std_msgs::msg::Bool>(enable_pub_topic, 10));
    RCLCPP_INFO(this->get_logger(), "Created publisher for topic: %s", enable_pub_topic.c_str());
  }
}

} // namespace ros_pub_sub_node

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(ros_pub_sub_node::RosPubSubNode)

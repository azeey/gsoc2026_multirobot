#include <chrono>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <ros_gz_interfaces/msg/world_statistics.hpp>
#include <ros_gz_interfaces/srv/control_world.hpp>
#include <ros_gz_interfaces/srv/delete_entity.hpp>
#include <ros_gz_interfaces/srv/set_entity_pose.hpp>
#include <ros_gz_interfaces/srv/spawn_entity.hpp>
#include <rosgraph_msgs/msg/clock.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <std_msgs/msg/bool.hpp>
#include <tf2_msgs/msg/tf_message.hpp>

class RosLoadNode : public rclcpp::Node
{
public:
  explicit RosLoadNode(const rclcpp::NodeOptions & options)
  : Node("overhead_ros_load", options)
  {
    const auto sensor_qos = rclcpp::SensorDataQoS();
    auto subscribe = [this, &sensor_qos](
      const std::string & topic, auto * message) {
        using Message = std::remove_pointer_t<decltype(message)>;
        subscriptions_.push_back(create_subscription<Message>(
          topic, sensor_qos, [](typename Message::ConstSharedPtr) {}));
      };

    for (int group = 1; group <= 4; ++group) {
      for (const char * name : {"banana", "grape", "orange"}) {
        const std::string prefix = "/vehicle_group_" + std::to_string(group) + "/" + name;
        subscribe(prefix + "/odometry", static_cast<nav_msgs::msg::Odometry *>(nullptr));
        subscribe(prefix + "/tf", static_cast<tf2_msgs::msg::TFMessage *>(nullptr));
        // The YAML bridge exposes sensors under these shorter ROS names.
        subscribe(prefix + "/imu", static_cast<sensor_msgs::msg::Imu *>(nullptr));
        subscribe(prefix + "/camera_info", static_cast<sensor_msgs::msg::CameraInfo *>(nullptr));
        subscribe(prefix + "/depth_image", static_cast<sensor_msgs::msg::Image *>(nullptr));
        subscribe(prefix + "/image", static_cast<sensor_msgs::msg::Image *>(nullptr));
        subscribe(prefix + "/points", static_cast<sensor_msgs::msg::PointCloud2 *>(nullptr));
        cmd_vel_pubs_.push_back(create_publisher<geometry_msgs::msg::Twist>(prefix + "/cmd_vel", 10));
        enable_pubs_.push_back(create_publisher<std_msgs::msg::Bool>(prefix + "/enable", 10));
      }

      for (int vehicle = 1; vehicle <= 3; ++vehicle) {
        const std::string prefix = "/world/multi_robot/model/vehicle_group_" +
          std::to_string(group) + "/model/vehicle_" + std::to_string(vehicle) +
          "/link/chassis/sensor";
        subscribe(prefix + "/imu_sensor/imu", static_cast<sensor_msgs::msg::Imu *>(nullptr));
        subscribe(prefix + "/rgbd_camera/camera_info", static_cast<sensor_msgs::msg::CameraInfo *>(nullptr));
        subscribe(prefix + "/rgbd_camera/depth_image", static_cast<sensor_msgs::msg::Image *>(nullptr));
        subscribe(prefix + "/rgbd_camera/image", static_cast<sensor_msgs::msg::Image *>(nullptr));
        subscribe(prefix + "/rgbd_camera/points", static_cast<sensor_msgs::msg::PointCloud2 *>(nullptr));
      }
    }

    subscribe("/tf", static_cast<tf2_msgs::msg::TFMessage *>(nullptr));
    subscribe("/world/multi_robot/clock", static_cast<rosgraph_msgs::msg::Clock *>(nullptr));
    subscribe("/world/multi_robot/dynamic_pose/info", static_cast<tf2_msgs::msg::TFMessage *>(nullptr));
    subscribe("/world/multi_robot/pose/info", static_cast<tf2_msgs::msg::TFMessage *>(nullptr));
    subscribe("/world/multi_robot/stats", static_cast<ros_gz_interfaces::msg::WorldStatistics *>(nullptr));

    const std::string world = "/world/multi_robot/";
    clients_.push_back(create_client<ros_gz_interfaces::srv::ControlWorld>(world + "control"));
    for (const char * name : {"create", "create/blocking"}) {
      clients_.push_back(create_client<ros_gz_interfaces::srv::SpawnEntity>(world + name));
    }
    for (const char * name : {
      "remove", "remove/blocking", "enable_collision", "enable_collision/blocking",
      "disable_collision", "disable_collision/blocking"})
    {
      clients_.push_back(create_client<ros_gz_interfaces::srv::DeleteEntity>(world + name));
    }
    for (const char * name : {"set_pose", "set_pose/blocking"}) {
      clients_.push_back(create_client<ros_gz_interfaces::srv::SetEntityPose>(world + name));
    }

    timer_ = create_wall_timer(std::chrono::milliseconds(100), [this] {
      geometry_msgs::msg::Twist twist;
      std_msgs::msg::Bool enabled;
      enabled.data = true;
      for (const auto & publisher : cmd_vel_pubs_) {
        publisher->publish(twist);
      }
      for (const auto & publisher : enable_pubs_) {
        publisher->publish(enabled);
      }
    });
  }

private:
  std::vector<rclcpp::SubscriptionBase::SharedPtr> subscriptions_;
  std::vector<rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr> cmd_vel_pubs_;
  std::vector<rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr> enable_pubs_;
  std::vector<rclcpp::ClientBase::SharedPtr> clients_;
  rclcpp::TimerBase::SharedPtr timer_;
};

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(RosLoadNode)

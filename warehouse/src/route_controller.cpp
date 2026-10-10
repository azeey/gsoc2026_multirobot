#include <array>
#include <chrono>
#include <cmath>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/pose_array.hpp>
#include <rclcpp/rclcpp.hpp>

using namespace std::chrono_literals;

namespace warehouse
{

class RouteController : public rclcpp::Node
{
public:
  explicit RouteController(const rclcpp::NodeOptions & options)
  : Node("warehouse_route_controller", options)
  {
    for (int index = 1; index <= 4; ++index)
    {
      const auto name = "robot" + std::to_string(index);
      const auto topic_namespace = declare_parameter<std::string>(
        name + ".namespace", "");
      if (topic_namespace.empty())
        continue;

      const auto waypoints = declare_parameter<std::vector<std::string>>(
        name + ".route", {});

      RobotRoute route;
      for (const auto & waypoint : waypoints)
      {
        std::array<double, 3> values;
        std::istringstream input(waypoint);
        std::string extra;
        if (!(input >> values[0] >> values[1] >> values[2]) ||
            input >> extra || !std::isfinite(values[0]) ||
            !std::isfinite(values[1]) || !std::isfinite(values[2]))
          throw std::runtime_error(name + ": waypoint must be 'x y yaw'");

        geometry_msgs::msg::Pose pose;
        pose.position.x = values[0];
        pose.position.y = values[1];
        pose.orientation.z = std::sin(values[2] / 2.0);
        pose.orientation.w = std::cos(values[2] / 2.0);
        route.goals.push_back(pose);
      }

      route.publisher = create_publisher<geometry_msgs::msg::PoseArray>(
        topic_namespace + "/cmd_pose", 10);
      route.subscriber = create_subscription<geometry_msgs::msg::Pose>(
        topic_namespace + "/reached_pose", 10,
        [this, name](const geometry_msgs::msg::Pose::SharedPtr) {
          on_reached(name);
        });
      RCLCPP_INFO(get_logger(), "%s (%s): loaded %zu goals",
        name.c_str(), topic_namespace.c_str(), route.goals.size());
      routes_.emplace(name, std::move(route));
    }

    timer_ = create_wall_timer(1s, [this]() { publish_pending_goals(); });
  }

private:
  struct RobotRoute
  {
    std::vector<geometry_msgs::msg::Pose> goals;
    size_t next_goal{0};
    bool sent{false};
    std::chrono::steady_clock::time_point last_sent{};
    rclcpp::Publisher<geometry_msgs::msg::PoseArray>::SharedPtr publisher;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr subscriber;
  };

  void send_goal(const std::string & name, RobotRoute & route)
  {
    geometry_msgs::msg::PoseArray command;
    command.header.stamp = get_clock()->now();
    command.poses.push_back(route.goals[route.next_goal]);
    route.publisher->publish(command);
    route.sent = true;
    route.last_sent = std::chrono::steady_clock::now();
  }

  void publish_pending_goals()
  {
    const auto now = std::chrono::steady_clock::now();
    for (auto & [name, route] : routes_)
    {
      if (route.next_goal == route.goals.size() ||
          route.publisher->get_subscription_count() == 0)
        continue;

      // Retry if startup or transport timing caused a goal message to be lost.
      if (route.sent && now - route.last_sent < 5s)
        continue;

      send_goal(name, route);
    }
  }

  void on_reached(const std::string & name)
  {
    auto & route = routes_.at(name);
    if (!route.sent || route.next_goal == route.goals.size())
      return;

    ++route.next_goal;
    if (route.next_goal == route.goals.size())
    {
      route.sent = false;
      RCLCPP_INFO(get_logger(), "%s: route complete", name.c_str());
      return;
    }

    send_goal(name, route);
  }

  std::map<std::string, RobotRoute> routes_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace warehouse

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(warehouse::RouteController)

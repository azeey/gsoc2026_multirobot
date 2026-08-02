cmake_minimum_required(VERSION 3.5)

project(auto_bridge_test)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/RosPubSubNode.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "ros_pub_sub_node::RosPubSubNode"
  EXECUTABLE ros_pub_sub_node
)

ament_auto_package(
  INSTALL_TO_SHARE
  config
  launch
  world
)


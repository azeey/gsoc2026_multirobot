cmake_minimum_required(VERSION 3.5)

project(topiclist_test)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/TopicListTestNode.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "topiclist_test_node::TopicListTestNode"
  EXECUTABLE topiclist_test_node
)

ament_auto_package(
  INSTALL_TO_SHARE
  launch
  world
)


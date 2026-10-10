cmake_minimum_required(VERSION 3.5)

project(warehouse)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/route_controller.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "warehouse::RouteController"
  EXECUTABLE warehouse_route_controller
)

ament_environment_hooks("${CMAKE_CURRENT_SOURCE_DIR}/hooks/${PROJECT_NAME}.dsv.in")

ament_auto_package(
  INSTALL_TO_SHARE
  config
  launch
  models
  worlds
)

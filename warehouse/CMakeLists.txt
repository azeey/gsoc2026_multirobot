cmake_minimum_required(VERSION 3.5)

project(warehouse)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_environment_hooks("${CMAKE_CURRENT_SOURCE_DIR}/hooks/${PROJECT_NAME}.dsv.in")

ament_auto_package(
  INSTALL_TO_SHARE
  config
  launch
  models
  worlds
)

cmake_minimum_required(VERSION 3.5)

project(demo_xacro)

find_package(ament_cmake REQUIRED)
find_program(XACRO_EXECUTABLE xacro REQUIRED)

set(generated_world_dir "${CMAKE_CURRENT_BINARY_DIR}/worlds")
set(generated_world "${generated_world_dir}/multi_robot.sdf")

add_custom_command(
  OUTPUT "${generated_world}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${generated_world_dir}"
  COMMAND "${XACRO_EXECUTABLE}"
          "${CMAKE_CURRENT_SOURCE_DIR}/worlds/multi_robot.sdf.xacro"
          -o "${generated_world}"
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/worlds/multi_robot.sdf.xacro"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/vehicle/model_xacro.sdf.xacro"
    "${CMAKE_CURRENT_SOURCE_DIR}/models/vehicle_group/model_xacro.sdf.xacro"
  VERBATIM
)

add_custom_target(generate_demo_xacro_world ALL
  DEPENDS "${generated_world}"
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

install(
  DIRECTORY
    launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

install(
  DIRECTORY
    models/
  DESTINATION share/${PROJECT_NAME}/models
)

install(
  DIRECTORY
    worlds/
  DESTINATION share/${PROJECT_NAME}/worlds
  FILES_MATCHING PATTERN "*.xacro"
)

install(
  FILES
    "${generated_world}"
  DESTINATION share/${PROJECT_NAME}/worlds
)

ament_environment_hooks("${CMAKE_CURRENT_SOURCE_DIR}/hooks/${PROJECT_NAME}.dsv.in")

ament_package()

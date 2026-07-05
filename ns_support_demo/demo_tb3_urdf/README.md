# Demo TB3 URDF

This demo is based on
[nav2_minimal_turtlebot_simulation](https://github.com/ros-navigation/nav2_minimal_turtlebot_simulation), especially the `nav2_minimal_tb3_sim` package. 

The original demo provides a minimal TurtleBot3 simulation for Nav2. This version changes the spawning flow to show Gazebo namespace support. 

Unlike other **SDF-focused** namespace demos (demo_ns_support, demo_original, demo_xacro), this package mainly uses **URDF** robot descriptions, showing that the same namespace support also works for **URDF-based** robots.

## What This Demo Shows

In the original version, each robot's Gazebo plugin topics must be expanded with the desired namespace in the robot description, by using xacro parameters to generate a different URDF result for each robot.

With the namespace support used by this demo, the robot description does not need to encode a separate topic namespace for each robot. The namespace is passed when the robot is created:

```python
Node(
    package='ros_gz_sim',
    executable='create',
    arguments=[
        '-name', robot_name,
        '-ns', namespace,
        '-file', robot_sdf,
        ...
    ],
)
```

This greatly reduces configuration complexity: adding or changing a robot namespace is handled by the launch argument, not by xacro. The same mechanism also works when the robot is spawned from a URDF file.

## Run

* Build the workspace, source it

* Run the regular TurtleBot3 simulation with:

  ```bash
  ros2 launch demo_tb3_urdf simulation.launch.py
  ```

  By default this starts one robot named `tb3` in the `tb3` namespace.

* Run the GPS variant with:

  ```bash
  ros2 launch demo_tb3_urdf simulation_gps.launch.py
  ```

  By default the GPS variant starts one robot named `tb3_gps` in the `tb3_gps` namespace.

## Check Topics

* Check Gazebo topics with:

  ```bash
  gz topic --list
  ```

* Check ros topics with:

  ```bash
  ros2 topic list
  ```

## Notes

This package still uses xacro during launch, for example to generate the world SDF.

The main difference is that xacro is no longer needed to set a different topic namespace for each robot model. The topic namespace is now set when spawning the robot, using the `namespace` launch argument.

# Demo Xacro

This demo uses xacro to generate SDF files with topic names passed as parameters.

The goal is to reduce the repeated SDF files used in `demo_original`. Instead of writing one SDF file for each vehicle and group, xacro macros build the final world with the needed topic names.

Each `vehicle_group` contains three vehicle models:

- `banana`
- `orange`
- `grape`

Each vehicle has these main topics:

- `imu`: IMU sensor output.
- `odometry`: odometry output from the `DiffDrive` plugin.
- `tf`: tf output from the `DiffDrive` plugin.
- `cmd_vel`: velocity command input for the `DiffDrive` plugin.
- `enable`: enable input for the `DiffDrive` plugin.

The world creates three groups in different ways:

- `vehicle_group_1` is defined directly in the world file.
- `vehicle_group_2` is spawned by the `ros_gz_sim create` node.
- `vehicle_group_3` is spawned by `gz_spawn_model`.

## Run & Check

* Build the package, source the workspace, and run:

  ```bash
  ros2 launch demo_xacro demo_xacro.launch.xml
  ```

* This launch file also starts `ros_gz_bridge` with the bridge configuration in `config/bridge.yaml`. The diff drive `enable` topics are not bridged because different robots will still publish conflicting Gazebo `enable` topic names.
 
  Check Gazebo topics with:

  ```bash
  gz topic --list
  ```

  Check ros topics with:

  ```bash
  ros2 topic list
  ```

## What This Demo Shows

The xacro files define topic names for each vehicle:

```text
/<group>/<vehicle>/imu
/<group>/<vehicle>/odometry
/<group>/<vehicle>/tf
/<group>/<vehicle>/cmd_vel
```

For example:

```text
/vehicle_group_1/banana/imu
/vehicle_group_1/banana/odometry
/vehicle_group_1/banana/tf
/vehicle_group_1/banana/cmd_vel
```

This keeps the main topic names short and separated by group.

## Limitations

* Topics that cannot be set by the user still use the default Gazebo topic name. In this demo, the diff drive `enable` topic can still conflict when the same nested model names are used in several groups.

* Xacro reduces repeated SDF text, but users must know xacro syntax and must build or generate the SDF before use.
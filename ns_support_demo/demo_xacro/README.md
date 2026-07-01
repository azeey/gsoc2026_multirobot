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

## Run

Build the package, source the workspace, and run:

```bash
ros2 launch demo_xacro demo_xacro.launch.xml
```

Check Gazebo topics with:

```bash
gz topic --list
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

* The reusable model is a xacro file, not a plain `vehicle_group.sdf`.That means it cannot be spawned directly with `ros_gz_sim create`, `gz_spawn_model`, or other command line tools that expect a ready SDF model. This makes it less flexible than direct namespace support.

* Xacro reduces repeated SDF text, but users must know xacro syntax and must build or generate the SDF before use.
# Demo Original

This demo shows the topic naming problem that appears when the same nested model is spawned more than once without namespace support.

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

## Run

Build the package, source the workspace, and run:

```bash
ros2 launch demo_original demo_original.launch.xml
```

To show the default naming problem, run:

```bash
ros2 launch demo_original demo_original_bug.launch.xml
```

Check Gazebo topics with:

```bash
gz topic --list
```

## Default Topic Naming Problem

* `demo_original_bug.launch.xml` spawns the same `vehicle_group/model.sdf` three times. The `DiffDrive` plugin builds its default topics using only the name of the model that directly contains the plugin, for example:

  ```text
  /model/vehicle_1/odometry
  /model/vehicle_1/tf
  /model/vehicle_1/cmd_vel
  /model/vehicle_1/enable
  /model/vehicle_2/odometry
  /model/vehicle_2/tf
  /model/vehicle_2/cmd_vel
  /model/vehicle_2/enable
  /model/vehicle_3/odometry
  /model/vehicle_3/tf
  /model/vehicle_3/cmd_vel
  /model/vehicle_3/enable
  ```
  
  Because every group contains models with the same names, topics from different groups are mixed together.

* IMU topics use the full scoped entity name when no topic is set. This avoids conflicts, but the names are long:

  ```text
  /world/multi_robot/model/vehicle_group_1/model/vehicle_1/link/chassis/sensor/imu_sensor/imu
  /world/multi_robot/model/vehicle_group_2/model/vehicle_1/link/chassis/sensor/imu_sensor/imu
  /world/multi_robot/model/vehicle_group_3/model/vehicle_1/link/chassis/sensor/imu_sensor/imu
  ```

## Manual Topic Workaround

* `demo_original.launch.xml` avoids most conflicts by using separate SDF files for each vehicle and by writing the topic names directly in those files. The main topics then use group names:

  ```text
  /vehicle_group_1/banana/imu
  /vehicle_group_1/banana/odometry
  /vehicle_group_1/banana/tf
  /vehicle_group_1/banana/cmd_vel
  /vehicle_group_2/orange/imu
  /vehicle_group_2/orange/odometry
  /vehicle_group_2/orange/tf
  /vehicle_group_2/orange/cmd_vel
  /vehicle_group_3/grape/imu
  /vehicle_group_3/grape/odometry
  /vehicle_group_3/grape/tf
  /vehicle_group_3/grape/cmd_vel
  ```

  This workaround is hard to maintain. Many SDF files are almost the same, and only the topic names are different.

* It also does not solve all topics. The diff drive `enable` topic cannot be set by the user in this setup, so it still uses default names:

  ```text
  /model/vehicle_1/enable
  /model/vehicle_2/enable
  /model/vehicle_3/enable
  ```


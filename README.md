# [GSoC 2026] Scalable Multi-Robot Simulation in ROS 2 and Gazebo

**Organization:** Open Robotics

**Contributor:** Jiayi Cai ([@C88-YQ](https://github.com/C88-YQ))

**Mentor:** Addisu Taddese ([@azeey](https://github.com/azeey))

**Community Meeting Talk**: [Scalable Multi-Robot Simulation in ROS 2 and Gazebo](https://discourse.openrobotics.org/t/community-meeting-scalable-multi-robot-simulation-in-ros-2-and-gazebo/58728)

## Project Overview

Multi-robot simulation is widely used in robotics research and development, including warehouse automation, robot swarms, and multi-robot coordination.

However, setting up a multi-robot simulation in ROS 2 and Gazebo can require a lot of repetitive configuration.

There are two main challenges:

1. **Communication isolation:** When multiple robots share the same SDF model, they may also share the same topic and service names, leading to conflicts. Users often need to create separate SDF files or use templates to configure different names for each robot.
2. **ROS-Gazebo bridge configuration:** Each robot usually requires several topic and service bridges. As the number of robots increases, manually maintaining these configurations becomes more difficult.

The goal of this GSoC project was to simplify multi-robot simulation by introducing two main features:

- **Model namespace support:** Allow multiple robots to share the same SDF model while keeping their communication interfaces separate.
- **Automated ROS-Gazebo bridging:** Automatically discover and create bridges for matching ROS 2 and Gazebo topics and services, reducing manual configuration.

These features aim to make multi-robot simulations easier to set up, maintain, and scale.

## Main Work

### 1. Model Namespace Support

The first major part of the project was introducing model namespace support across SDFormat, Gazebo Sim, and ROS-Gazebo integration.

With this feature, users can assign a namespace to a model, allowing supported Gazebo plugins to use it when resolving topic and service names.

Namespaces can be configured directly in SDF files using the `<model>` namespace attribute or the `<include>` namespace element. They can also be specified when spawning models at runtime.

For nested models, namespaces are resolved by searching from the current model up through its parents, combining relative namespaces until an absolute namespace is found. The resolved namespace is then used as a prefix for default and relative topic and service names, while absolute names remain unchanged.

**Backward compatibility** is preserved, so models without a namespace continue to use their existing topic and service naming behavior.

This feature provides a consistent approach for handling topic and service names across Gazebo plugins. It also allows multiple robots to share the same SDF model while keeping their communication interfaces separate, making multi-robot simulations much easier to set up and maintain.

Namespace support has been added to DiffDrive and extended to other drive plugins and sensors, with several PRs still under review.

### 2. Automated ROS-Gazebo Bridge

Previously, users needed to manually configure the names, message types, and communication directions of their bridges. This becomes repetitive when working with many robots that have similar communication interfaces.

The new automated bridge can discover matching topics and services on the ROS 2 and Gazebo sides and create the corresponding bridges.

The feature is optional and can be enabled using the `automated_bridge.enable` parameter. It also provides two optional configuration parameters:

- `include_patterns`: Select which topics and services should be considered.
- `exclude_patterns`: Exclude specific topics and services from automated bridging.

The automated bridge continuously checks for new interfaces, allowing it to discover topics and services created after the simulation starts.

It also supports using manual and automated bridge configurations together. Within the same bridge instance, manually configured bridges take priority, and automated bridging handles the remaining interfaces.

To evaluate the overhead of continuous discovery and matching, I tested the automated bridge with a simulation containing 12 robots, 124 Gazebo topics, and 94 Gazebo services, along with a separate ROS 2 node providing 115 topics and 44 services. With actual bridge creation disabled during this test, average CPU usage increased from **2.44% to 3.32%**, while memory usage remained nearly unchanged. This suggests that the discovery and matching process adds relatively little overhead in the tested setup.

### 3. Documentation and Demos

Three documentation updates and two demos were prepared to demonstrate the new features and help users get started.

**Documentation:**

- [Model Namespace](https://github.com/gazebosim/docs/pull/719) — How to configure and use model namespaces.
- [Automated Bridge](https://github.com/gazebosim/docs/pull/721) — How to enable and configure automated bridging.
- [Multi-Robot Tutorial](https://github.com/gazebosim/docs/pull/722) — How to use both features together in multi-robot simulations.

**Demos:**

- [Multi-Robot Demo](https://github.com/gazebosim/ros_gz/pull/978) — Demonstrates model namespaces and automated bridging with multiple robots.
- [Warehouse Demo](https://github.com/azeey/gsoc2026_multirobot/tree/main/warehouse) — Demonstrates independent control of four Mecanum-wheel robots sharing the same SDF model.



## Project Status and Pull Requests

The project involved changes across several repositories, including `sdformat`, `gz-msgs`, `gz-sim`, `gz-transport`, `ros_gz`, and Gazebo documentation. As of October 27, 2026, the core namespace infrastructure and several related features have been merged, while some plugin integrations, automated bridging, demos, and documentation are still under review.

The following table lists all PRs for the project:

<table>
  <thead>
    <tr>
      <th>Feature</th>
      <th>PR</th>
      <th>Title</th>
      <th>Status</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td rowspan="20">Namespace Support</td>
      <td><a href="https://github.com/gazebosim/sdformat/pull/1663">sdformat #1663</a></td>
      <td>Add support for namespace attribute on &lt;model&gt;</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/3777">gz-sim #3777</a></td>
      <td>Add namespace component</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/3872">gz-sim #3872</a></td>
      <td>Display namespace in gui</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/3897">gz-sim #3897</a></td>
      <td>Add helper functions for namespaced topic resolution and unit test setup</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/3871">gz-sim #3871</a></td>
      <td>Support entity_namespace in user commands entity creation</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-msgs/pull/595">gz-msgs #595</a></td>
      <td>Add EntityFactoryWithNs</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-msgs/pull/612">gz-msgs #612</a></td>
      <td>Add entity_namespace to EntityFactory</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-msgs/pull/616">gz-msgs #616</a></td>
      <td>Deprecate EntityFactoryWithNs and EntityFactoryWithNs_V</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/4057">gz-sim #4057</a></td>
      <td>UserCommands: Deprecate EntityFactoryWithNs and EntityFactoryWithNs_V</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/ros_gz/pull/925">ros_gz #925</a></td>
      <td>Add entity_namespace support for spawned entities</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/ros_gz/pull/1005">ros_gz #1005</a></td>
      <td>Replace EntityFactoryWithNs with EntityFactory for entity creation</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/3878">gz-sim #3878</a></td>
      <td>Add namespace support to DiffDrive</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/4004">gz-sim #4004</a></td>
      <td>DiffDrive: Add support for custom enable topic</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/4067">gz-sim #4067</a></td>
      <td>Document namespace support in DiffDrive parameters</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/4066">gz-sim #4066</a></td>
      <td>MecanumDrive: Add namespace support</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/4136">gz-sim #4136</a></td>
      <td>AckermannSteering: Add namespace support</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/4041">gz-sim #4041</a></td>
      <td>Add namespace support to rendering sensor</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/4050">gz-sim #4050</a></td>
      <td>Add namespace support to sensor plugins</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/gz-sim/pull/4052">gz-sim #4052</a></td>
      <td>Add namespace support to sensor plugins</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/docs/pull/719">docs #719</a></td>
      <td>Document namespace support</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td rowspan="3">Automated Bridge</td>
      <td><a href="https://github.com/gazebosim/gz-transport/pull/967">gz-transport #967</a></td>
      <td>Add Node::AllTopicInfo</td>
      <td>✅ Merged</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/ros_gz/pull/923">ros_gz #923</a></td>
      <td>Support automated bridging</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/docs/pull/721">docs #721</a></td>
      <td>Document automated bridge</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td rowspan="2">Demo</td>
      <td><a href="https://github.com/gazebosim/ros_gz/pull/978">ros_gz #978</a></td>
      <td>Add multi-robot namespace and automated bridge demo</td>
      <td>🔄 Under review</td>
    </tr>
    <tr>
      <td><a href="https://github.com/gazebosim/docs/pull/722">docs #722</a></td>
      <td>Add multi-robot tutorial with namespaces and automated bridging</td>
      <td>🔄 Under review</td>
    </tr>
  </tbody>
</table>

## Future Work

**1. Extending Namespace Support**

Namespace support will continue to be extended to the remaining Gazebo plugins and sensors. Some PRs are already under review, and additional issues have been prepared for community contributors.

**2. Handling TF Frame IDs**

Although namespaces can separate TF topics for different robots, the frame IDs inside TF messages may still conflict.

Two possible approaches are to maintain independent TF trees for each robot or rewrite frame IDs on the ROS 2 side before combining them. Future work will explore supporting namespace-based frame ID prefixes directly in Gazebo.

**3. Further Improvements to Automated Bridging**

The discovery and direction inference logic developed for automated bridging could also be used to simplify manual bridge configuration, reducing the need to specify message types and bridge directions manually.

## Acknowledgments

Sincere thanks to my mentor, Addisu Taddese, for his excellent guidance and support throughout the project, and to the Gazebo and Open Robotics community members for their valuable feedback, reviews, and suggestions.

Working on this project has been a great learning experience, especially in open-source development and collaboration. I'm grateful to Google Summer of Code and Open Robotics for this opportunity and look forward to continuing my contributions to Gazebo.
# Test Report

## Test Environment OS

  * Ubuntu 24.04.4 LTS 
  * Kernel: Linux 6.8.0-124-generic (PREEMPT_DYNAMIC) 
  * CPU: Intel(R) Core(TM) Ultra 5 125H, 14 cores / 18 logical CPUs 
  * Memory: 30 GiB RAM 
  * Architecture: x86_64

## Test Setup

A controlled multi-robot Gazebo world is used to evaluate the overhead of automatic bridge creation. 

The world contains 12 robots, and each robot is equipped with an IMU sensor and an RGB-D camera.

 Each robot subscribes to the following topics:
- `/cmd_vel`
- `/enable` 

And publishes the following topics:
- `/odometry`
- `/tf`
- `/imu`
- `/camera_info`
- `/image`
- `/depth_image`
- `/points`


Two launch files are used for comparison:

1. **overhead_test_original.launch.py**

  * Creates bridges from a YAML configuration file.
  * Mainly covers all robot-related topics (listed above).

2. **overhead_test_dynamic.launch.py**
  * Enables `create_dynamic_bridges:=true`.
  * Automatically creates bridges for all Gazebo-published topics.

Compared with the selective bridge launch, the automatic bridge launch also creates bridges for several non-robot-related topics, including:

```bash
/world/multi_robot/clock
/world/multi_robot/dynamic_pose/info
/world/multi_robot/pose/info
/world/multi_robot/stats
/clock
/gazebo/resource_paths
/gui/camera/pose
/stats
```

---
## Part1: Baseline
### 1. Overhead Test Result
1. Using the existing selective YAML bridge:
  * Average CPU: **2.64%**
  * Average RSS: **148.7 MB**

2. Using the automatic bridge creation from this PR:
  * Average CPU: **4.82%**
  * Average RSS: **144.3 MB**

### 2. Conculusion
* The runtime overhead appears to be acceptable. Although automatic bridge creation increases CPU usage, the increase is relatively modest in this test scenario.
* One concern is that the current approach creates bridges for every Gazebo-published topic, including topics that appear to be internal to Gazebo or unrelated to ROS (e.g. `/gazebo/resource_paths`, `/gui/camera/pose`).

---
## Part2: Additional Investigation for Gazebo Subscriber Topics

### 1. Changes
The current PR does not fully handle Gazebo subscriber-only topics during dynamic bridge creation. To continue the evaluation, a local modification was added in `ros_gz_bridge/src/ros_gz_bridge.cpp` inside `void RosGzBridge::add_dynamic_bridges()`.

The main change is that dynamic bridge discovery now also inspects the `subscribers` returned by `gz_node_->TopicInfo(...)`, in addition to the existing `publishers`:

```cpp
for (const auto & subscriber : subscribers) {
  const auto gz_type_name = subscriber.MsgTypeName();
  if (!gz_type_name.empty()) {
    gz_type_names.insert(gz_type_name);
  }
}
```

This change is necessary because topics such as `/cmd_vel` and `/enable` are subscriber-facing from the Gazebo side. If only publishers are checked, these topics may never contribute a Gazebo message type and therefore cannot be bridged dynamically.

### 2. Observation: `TopicList()` Is Incomplete

The topic list obtained through `gz_node_->TopicList(...)` is not complete. In practice, the missing topics are mainly subscriber-related topics such as `/cmd_vel` and `/enable`, plus some other non-robot-related topics.

Further testing shows that introducing a short delay in `gz-transport/src/Discovery.hh` inside `TopicList(std::vector<std::string> &_topics)` (add [position](https://github.com/gazebosim/gz-transport/blob/11b728b6c7073cde958a7f28cc9952da19fcb697/src/Discovery.hh#L830))

helps the transport layer collect a more complete topic list before returning it.

Under the current setup:

* Adding a **5 ms** sleep makes the returned topic list almost complete.
* Adding a **1 ms** sleep still does not make the topic list fully complete, but it does expose more topics than the original implementation.
* With the 1 ms delay, once bridges are created for the newly discovered subscriber-related topics, the system may eventually converge. After roughly 20 repeated attempts, the topic list can become complete and the remaining bridges are then created.

### 3. Reason: Cause of Incomplete `TopicList()` Results

The incomplete result is most likely caused by the asynchronous nature of Gazebo Transport discovery.

`Discovery::TopicList()` does not read from a fully synchronized global registry. Instead, it:

1. clears the cached remote subscriber information,
2. sends a `SUBSCRIBERS_REQ` discovery message,
3. waits for initialization,
4. waits only a short fixed time,
5. reads local topic information and the currently received remote subscriber cache.

This means `TopicList()` is effectively taking a snapshot while discovery replies are still in flight. Subscriber-only topics are especially sensitive to this because they depend on remote subscriber discovery data rather than only on local `info` storage ([private: TopicStorage<Pub> info](https://github.com/gazebosim/gz-transport/blob/11b728b6c7073cde958a7f28cc9952da19fcb697/src/Discovery.hh#L1702)).

As a result, the returned topic list can be incomplete because discovery replies are processed asynchronously, so not all subscriber responses may have arrived before `TopicList()` reads the cache.

In another words, the issue is not only in bridge creation logic. It is also a timing and synchronization limitation in the current discovery-based implementation of `TopicList()`.

### 4. Overhead Test Result

Although adding sleep improves topic discovery completeness, it also increases CPU overhead significantly.

1. Sleep 1 ms

   * Average CPU: **10.04%**
   * Average RSS: **153.7 MB**

2. Sleep 5 ms

   * Average CPU: **23.01%**
   * Average RSS: **154.1 MB**

Compared with the baseline dynamic bridge result:

* The **1 ms** delay already more than doubles CPU usage.
* The **5 ms** delay increases CPU usage even further 

### 5. Conclusion

* The current automatic bridge creation approach can discover and create bridges for more topics after extending `add_dynamic_bridges()` to inspect Gazebo subscribers. However, the final result is still limited by incomplete topic discovery in `gz::transport::Discovery::TopicList()`.

* Adding a small delay inside `TopicList()` improves completeness, especially for subscriber-related topics such as `/cmd_vel` and `/enable`, but this comes at a significant CPU cost. Therefore, a fixed sleep is useful as a diagnostic workaround, but it is not a good long-term solution for production use.
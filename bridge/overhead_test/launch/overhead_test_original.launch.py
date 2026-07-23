import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node
from ros_gz_bridge.actions import RosGzBridge
from ros_gz_sim.actions import GzServer
from ros_gz_sim.actions import GzSpawnModel


def generate_launch_description():
    pkg_share = get_package_share_directory('overhead_test')
    world_file = os.path.join(pkg_share, 'worlds', 'multi_robot.sdf')
    vehicle_group_file = os.path.join(pkg_share, 'models', 'vehicle_group', 'model.sdf')
    bridge_config = os.path.join(pkg_share, 'config', 'bridge.yaml')

    actions = [
        GzServer(
            world_sdf_file=world_file,
            use_composition=False,
            create_own_container=True,
        ),
        RosGzBridge(
            bridge_name='ros_gz_bridge',
            config_file=bridge_config,
            use_composition=False,
        ),
    ]

    actions.append(ExecuteProcess(cmd=['gz', 'sim', '-g'], output='screen'))

    return LaunchDescription(actions)

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node
from ros_gz_bridge.actions import RosGzBridge
from ros_gz_sim.actions import GzServer
from ros_gz_sim.actions import GzSpawnModel


def generate_launch_description():
    pkg_share = get_package_share_directory('demo_original')
    world_file = os.path.join(pkg_share, 'worlds', 'multi_robot.sdf')
    vehicle_group_2_file = os.path.join(pkg_share, 'models', 'vehicle_group_2', 'model.sdf')
    vehicle_group_3_file = os.path.join(pkg_share, 'models', 'vehicle_group_3', 'model.sdf')
    bridge_config = os.path.join(pkg_share, 'config', 'bridge.yaml')

    actions = [
        GzServer(
            world_sdf_file=world_file,
            use_composition=True,
            create_own_container=True,
        ),
        Node(
            package='ros_gz_sim',
            executable='create',
            arguments=[
                '-world', 'multi_robot',
                '-file', vehicle_group_2_file,
                '-name', 'vehicle_group_2',
                '-ns', '{name}',
                '-x', '3.0',
                '-y', '0.0',
                '-z', '0.0',
                '-Y', '0.0',
            ],
            output='screen',
        ),
        GzSpawnModel(
            world='multi_robot',
            file=vehicle_group_3_file,
            entity_name='vehicle_group_3',
            entity_namespace='{name}',
            allow_renaming='false',
            x='6.0',
            y='0.0',
            z='0.0',
            yaw='0.0',
        ),
    ]

    groups = ('vehicle_group_1', 'vehicle_group_2', 'vehicle_group_3')
    vehicles = ('banana', 'orange', 'grape')

    actions.extend(
        RosGzBridge(
            bridge_name=f'ros_gz_bridge_{group}_{vehicle}',
            namespace=f'/{group}/{vehicle}',
            config_file=bridge_config,
            use_composition=True,
            bridge_params=[{'expand_gz_topic_names': True}],
        )
        for group in groups
        for vehicle in vehicles
    )

    actions.append(ExecuteProcess(cmd=['gz', 'sim', '-g'], output='screen'))

    return LaunchDescription(actions)

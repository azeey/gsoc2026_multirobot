import os

from launch import LaunchDescription
from ament_index_python.packages import get_package_share_directory
from launch.actions import DeclareLaunchArgument, ExecuteProcess, TimerAction
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, LoadComposableNodes
from launch_ros.descriptions import ComposableNode
from ros_gz_sim.actions import GzServer
from ros_gz_bridge.actions import RosGzBridge

def generate_launch_description():
    pkg_share = get_package_share_directory('auto_bridge_test')
    world_file = os.path.join(pkg_share, 'world', 'world.sdf')
    config_file = os.path.join(pkg_share, 'config', 'config.yaml')
    bridge_file = os.path.join(pkg_share, 'config', 'bridge.yaml')
    use_composition = LaunchConfiguration('use_composition')
    freq = LaunchConfiguration('freq', default='1.0')
    container_name = 'ros_gz_container'

    declare_use_composition = DeclareLaunchArgument(
        'use_composition',
        default_value='False'
    )
    declare_freq = DeclareLaunchArgument(
        'freq',
        default_value='1.0'
    )

    gz_server = GzServer(
        world_sdf_file=world_file,
        container_name=container_name,
        use_composition=use_composition,
        create_own_container='True'
    )

    gz_gui = ExecuteProcess(
        cmd=['gz', 'sim', '-g'],
        output='screen'
    )

    ros_gz_bridge = RosGzBridge(
        bridge_name='ros_gz_bridge',
        config_file=bridge_file,
        use_composition=use_composition,
        container_name=container_name,
        bridge_params =[
            {'expand_gz_topic_names': False},
            {'enable_automated_bridge': True}
        ]
    )

    load_node = TimerAction(
        period=5.0,
        condition=UnlessCondition(use_composition),
        actions=[
            Node(
                package='auto_bridge_test',
                executable='ros_pub_sub_node',
                name='ros_pub_sub_node',
                output='screen',
                parameters=[config_file]
            )
        ]
    )

    load_composable_node = TimerAction(
        period=5.0,
        condition=IfCondition(use_composition),
        actions=[
            LoadComposableNodes(
                target_container=container_name,
                composable_node_descriptions=[
                    ComposableNode(
                        package='auto_bridge_test',
                        plugin='ros_pub_sub_node::RosPubSubNode',
                        name='ros_pub_sub_node',
                        parameters=[config_file]
                    )
                ]
            )
        ]
    )

    ld = LaunchDescription()
    ld.add_action(declare_use_composition)
    ld.add_action(declare_freq)
    ld.add_action(gz_server)
    ld.add_action(gz_gui)
    ld.add_action(ros_gz_bridge)
    ld.add_action(load_node)
    ld.add_action(load_composable_node)
    return ld
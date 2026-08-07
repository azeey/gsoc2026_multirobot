## Add

``` bash
export VEHICLE_SDF="$(ros2 pkg prefix --share ros_gz_sim_demos)/models/vehicle/model.sdf"
gz service -s /world/default/create/blocking \
    --reqtype gz.msgs.EntityFactory \
    --reptype gz.msgs.Boolean \
    --timeout 5000 \
    --req 'sdf_filename: "'"$VEHICLE_SDF"'",
           name: "robot2",
           pose: {
             position: {x: 0.0, y: 14.0, z: 1.0},
             orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}
           }'
```
## Remove

```
ros2 run ros_gz_bridge parameter_bridge /world/default/remove@ros_gz_interfaces/srv/DeleteEntity

ros2 run ros_gz_sim delete_entity --name robot2
```
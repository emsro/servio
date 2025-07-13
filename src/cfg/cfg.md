

# Config fields

<!-- GEN BEGIN HERE -->

### 1 `model`
Model of the servomotor, used for debugging purposes
 - _type_: `string_type`
 - _default_: `"no model"`

### 2 `id`
ID of the servomotor
 - _type_: `uint32_t`

### 3 `group_id`
Group ID of the servomotor
 - _type_: `uint32_t`

### 4 `encoder_mode`
Encoder mode of the servomotor
 - _type_: `cfg::encoder_mode`
 - _default_: `cfg::encoder_mode::analog`

### 11 `position_low_angle`
Expected lowest position of the servomotor
 - _type_: `float`
 - _unit_: rad

### 12 `position_high_angle`
Expected highest position of the servomotor
 - _type_: `float`
 - _unit_: rad
 - _default_: `2*pi`

### 14 `current_conv_scale`
Current conversion scale factor for formula: C = ADC * scale + offset
 - _type_: `float`
 - _default_: `1.0`

### 15 `current_conv_offset`
Current conversion offset for formula: C = ADC * scale + offset
 - _type_: `float`

### 16 `temp_conv_scale`
Temperature conversion scale factor for formula: T = ADC * scale + offset
 - _type_: `float`
 - _default_: `1.0`

### 17 `temp_conv_offset`
Temperature conversion offset for formula: T = ADC * scale + offset
 - _type_: `float`

### 18 `voltage_conv_scale`
Voltage conversion scale factor for formula: V = ADC * scale
 - _type_: `float`
 - _default_: `1.0`

### 19 `invert_hbridge`
Invert direction of the H-bridge
 - _type_: `bool`
 - _default_: `false`

### 30 `current_loop_p`

 - _type_: `float`
 - _default_: `1.0`

### 31 `current_loop_i`

 - _type_: `float`
 - _default_: `0.0001F`

### 32 `current_loop_d`

 - _type_: `float`

### 33 `current_lim_min`
Soft limit for lowest current
 - _type_: `float`
 - _default_: `-2.0`

### 34 `current_lim_max`
Soft limit for highest current
 - _type_: `float`
 - _default_: `2.0`

### 40 `velocity_loop_p`

 - _type_: `float`
 - _default_: `1.0`

### 41 `velocity_loop_i`

 - _type_: `float`
 - _default_: `0.000'000'2F`

### 42 `velocity_loop_d`

 - _type_: `float`

### 43 `velocity_lim_min`
Soft limit for lowest velocity
 - _type_: `float`
 - _unit_: rad/s
 - _default_: `-3.0`

### 44 `velocity_lim_max`
Soft limit for highest velocity
 - _type_: `float`
 - _unit_: rad/s
 - _default_: `3.0`

### 45 `velocity_to_current_lim_scale`
Scaling of current limit derived from velocity limit
 - _type_: `float`
 - _default_: `2.0`

### 50 `position_loop_p`

 - _type_: `float`
 - _default_: `0.2F`

### 51 `position_loop_i`

 - _type_: `float`
 - _default_: `0.000'000'02F`

### 52 `position_loop_d`

 - _type_: `float`

### 53 `position_lim_min`

 - _type_: `float`
 - _unit_: rad
 - _default_: `0.1F`

### 54 `position_lim_max`

 - _type_: `float`
 - _unit_: rad
 - _default_: `2.0F * pi - 0.1F`

### 55 `position_to_velocity_lim_scale`
Scaling of velocity limit derived from position limit
 - _type_: `float`
 - _unit_: rad
 - _default_: `2.0`

### 60 `static_friction_scale`
Scaling of current in case of non-moving servo
 - _type_: `float`
 - _default_: `3.0`

### 61 `static_friction_decay`
Time required for the static friction compensation to decay to be ineffective
 - _type_: `float`
 - _unit_: s
 - _default_: `1.0`

### 62 `minimum_voltage`
Minimum voltage for the servomotor to operate
 - _type_: `float`
 - _unit_: voltage
 - _default_: `6.0`

### 65 `maximum_temperature`
Maximum temperature for the servomotor to operate
 - _type_: `float`
 - _unit_: celsius
 - _default_: `80.0`

### 66 `moving_detection_step`
Deadband for the moving detection
 - _type_: `float`
 - _unit_: rad
 - _default_: `0.05F`

### 80 `quad_encoder_range`
Number of pulses per revolution of the quadrature encoder
 - _type_: `uint32_t`
 - _default_: `2048`
         <!-- GEN END HERE -->
